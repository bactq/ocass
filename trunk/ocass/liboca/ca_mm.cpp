/*
 * Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 * This file is part of ocass
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "ca_mm.h"
#include "ca_misc.h"
#include "ca_str.h"

typedef struct _CA_ShmBlk
{
    DWORD nShmRealSize;
    DWORD nShmSize;
} CAShmBlk;

struct _CA_SHMM_
{  
    TCHAR szFileName[MAX_PATH];
    DWORD nShmSize;

    HANDLE hMap;
    LPVOID pShmBase;
    CAShmBlk *pShmBlk;
};

CA_DECLARE(void*) CA_MAlloc(UINT nSize)
{
    return malloc(nSize);
}

CA_DECLARE(void) CA_MFree(void *pMem)
{
    if (NULL == pMem)
    {
        return;
    }

    free(pMem);
}

CA_DECLARE(CAErrno) CA_ShMMCreate(UINT nReqSize, const TCHAR *pszFName, 
                                 CAShMM **pShMM)
{
    LPCTSTR pszMapKey;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    LPVOID pMapView = NULL;
    CAShMM *pNewShMM = NULL;
    TCHAR szMapKey[MAX_PATH * 2];
    UINT nRealSize = nReqSize + sizeof(CAShmBlk);
    int nResult;

    *pShMM = NULL;
    if (NULL == pszFName)
    {
        hFile = INVALID_HANDLE_VALUE;
        pszMapKey = NULL;
    }
    else
    {
        hFile = CreateFile(pszFName, GENERIC_READ|GENERIC_WRITE, 
            FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }

        caErr = CA_W32ResNameFromFilename(pszFName, TRUE, 
            szMapKey, sizeof(szMapKey) / sizeof(szMapKey[0]));
        if (CA_ERR_SUCCESS != caErr)
        {
            funcErr = caErr;
            goto EXIT;
        }
        pszMapKey = szMapKey;
    }

    hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 
        0, nRealSize, pszMapKey);
    if (NULL == hMap)
    {
        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

    pMapView = MapViewOfFile(hMap, FILE_MAP_READ|FILE_MAP_WRITE,
                         0, 0, nRealSize);
    if (NULL == pMapView)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    if (INVALID_HANDLE_VALUE != hFile)
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }

    pNewShMM = (CAShMM*)CA_MAlloc(sizeof(CAShMM));
    if (NULL == pNewShMM)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    pNewShMM->hMap = hMap;
    pNewShMM->nShmSize = nRealSize;
    pNewShMM->pShmBase = pMapView;
    pNewShMM->pShmBlk = (CAShmBlk *)pMapView;
    pNewShMM->pShmBlk->nShmRealSize = nRealSize;
    pNewShMM->pShmBlk->nShmSize = nReqSize;
    nResult = CA_SNPrintf(pNewShMM->szFileName, 
        sizeof(pNewShMM->szFileName) / sizeof(pNewShMM->szFileName[0]), 
        TEXT("%s"), pszFName);
    if (0 >= nResult)
    {
        funcErr = CA_ERR_FNAME_TOO_LONG;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        /* failed close all handle */
        if (NULL != pMapView)
        {
            UnmapViewOfFile(pMapView);
        }

        if (NULL != hMap)
        {
            CloseHandle(hMap);
        }

        if (INVALID_HANDLE_VALUE != hFile)
        {
            CloseHandle(hFile);
        }

        if (NULL != pNewShMM)
        {
            CA_MFree(pNewShMM);
        }
    }
    else 
    {
        *pShMM = pNewShMM;
    }
    return funcErr;
}

CA_DECLARE(void) CA_ShMMDestroy(CAShMM *pShMM, BOOL bUnlinkMapFile)
{
    if (NULL == pShMM)
    {
        return;
    }

    if (NULL != pShMM->pShmBase)
    {
        UnmapViewOfFile(pShMM->pShmBase);
        pShMM->pShmBase = NULL;
    }

    if (NULL != pShMM->hMap)
    {
        CloseHandle(pShMM->hMap);
        pShMM->hMap = NULL;
    }

    if (bUnlinkMapFile)
    {
        DeleteFile(pShMM->szFileName);
        pShMM->szFileName[0] = '\0';
    }

    CA_MFree(pShMM);
}

CA_DECLARE(CAErrno) CA_ShMMAttach(const TCHAR *pszFileName, CAShMM **pShm)
{
    CAShmBlk *pShmBlk;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    LPVOID pMapView = NULL;
    HANDLE hMap = NULL;
    CAShMM *pNewShm = NULL;
    TCHAR szMapKey[MAX_PATH * 2];
    int nResult;

    *pShm = NULL;
    if (NULL == pszFileName || '\0' == pszFileName[0])
    {
        return CA_ERR_BAD_ARG;
    }
    
    caErr = CA_W32ResNameFromFilename(pszFileName, TRUE, 
            szMapKey, sizeof(szMapKey) / sizeof(szMapKey[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    hMap = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, szMapKey);
    if (NULL == hMap)
    {
        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

    pMapView = MapViewOfFile(hMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
    if (NULL == pMapView)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* verify memory/file size ??? */

    pShmBlk = (CAShmBlk *)pMapView;
    pNewShm = (CAShMM *)CA_MAlloc(sizeof(CAShMM));
    if (NULL == pNewShm)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    pNewShm->hMap = hMap;
    pNewShm->nShmSize = pShmBlk->nShmRealSize;
    pNewShm->pShmBase = pMapView;
    pNewShm->pShmBlk = pShmBlk;
    nResult = CA_SNPrintf(pNewShm->szFileName, 
        sizeof(pNewShm->szFileName) / sizeof(pNewShm->szFileName[0]), 
        TEXT("%s"), pszFileName);
    if (0 >= nResult)
    {
        funcErr = CA_ERR_FNAME_TOO_LONG;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        /* attach failed */
        if (NULL != pMapView)
        {
            UnmapViewOfFile(pMapView);
        }

        if (NULL != hMap)
        {
            CloseHandle(hMap);
        }

        if (NULL != pNewShm)
        {
            CA_MFree(pNewShm);
        }
    }
    else
    {
        *pShm = pNewShm;
    }

    return funcErr;
}

CA_DECLARE(void) CA_ShMMDettach(CAShMM *pShm)
{
    CA_ShMMDestroy(pShm, FALSE);
}

CA_DECLARE(void *) CA_ShMMBaseAddrGet(CAShMM *pShm)
{
    char *pBaseAddr = (char *)pShm->pShmBase;

    pBaseAddr += sizeof(CAShmBlk);
    return (void *)pBaseAddr;
}

CA_DECLARE(UINT) CA_ShMMSizeGet(CAShMM *pShm)
{
    return (pShm->nShmSize - sizeof(CAShmBlk));
}
