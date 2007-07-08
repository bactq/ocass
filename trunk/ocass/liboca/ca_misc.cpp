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

#include "liboca.h"
#include "ca_misc.h"
#include "ca_str.h"
#include "ca_tlhlp.h"

CA_DECLARE(CAErrno) CA_GetModPath(HMODULE hMod, 
                                  TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    TCHAR szModFName[MAX_PATH];
    TCHAR *pSlash;
    DWORD dwResult;
    int nResult;

    dwResult = GetModuleFileName(hMod, szModFName, 
        sizeof(szModFName) / sizeof(szModFName[0]));
    if (0 == dwResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    pSlash = strrchr(szModFName, '\\');
    if (NULL == pSlash)
    {
        return CA_ERR_FNAME_BAD;
    }

    pSlash[0] = '\0';
    nResult = CA_SNPrintf(pszFNameBuf, dwBufCnt, TEXT("%s"), szModFName);
    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

CA_DECLARE(CAErrno) CA_PathJoin(const TCHAR *pszPath, 
                                const TCHAR *pszSubPath, 
                                TCHAR *pszPathBuf, DWORD dwBufCnt)
{
    int nResult;

    nResult = CA_SNPrintf(pszPathBuf, dwBufCnt, TEXT("%s\\%s"), 
        pszPath, pszSubPath);

    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

CA_DECLARE(void) CA_PathGetBaseName(const TCHAR *pszFullName, 
                                    const TCHAR **pszBaseName)
{
    TCHAR *pszLastSlash;

    if (NULL == pszFullName || '\0' == pszFullName[0])
    {
        *pszBaseName = pszFullName;
        return;
    }

    pszLastSlash = strrchr(pszFullName, '\\');
    if (NULL == pszLastSlash)
    {
        *pszBaseName = pszFullName;
        return;
    }

    *pszBaseName = pszLastSlash + sizeof(TCHAR);
}

CA_DECLARE(CAErrno) CA_GetProcFirstThread(DWORD dwProcId, 
                                          DWORD *pdwFirstThId)
{
    THREADENTRY32 entThread;
    BOOL bResult;
    BOOL bFind;

    CToolhelp toolHelp(TH32CS_SNAPTHREAD, dwProcId);
    entThread.dwSize = sizeof(entThread);
    bResult = toolHelp.ThreadFirst(&entThread);
    if (!bResult)
    {
        return CA_ERR_FATAL;
    }

    for (bFind = FALSE;;)
    {        
        if (entThread.th32OwnerProcessID == dwProcId)
        {
            bFind = TRUE;
            break;
        }

        bResult = toolHelp.ThreadNext(&entThread);
        if (!bResult)
        {
            break;
        }
    }
    if (!bFind)
    {
        return CA_ERR_FATAL;
    }

    *pdwFirstThId = entThread.th32ThreadID;
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_W32ResNameFromFilename(const TCHAR *pszFName, 
                        BOOL bIsGlobal, TCHAR *pszNameBuf, DWORD dwBufCnt)
{
    const TCHAR *pszPre = (bIsGlobal ? TEXT("Global\\") : TEXT("Local\\"));
    TCHAR *pszStrPos;
    int nResult;

    if (NULL == pszFName || '\0' == pszFName[0])
    {
        return CA_ERR_BAD_ARG;
    }

    nResult = CA_SNPrintf(pszNameBuf, dwBufCnt, TEXT("%s%s"), 
        pszPre, pszFName);
    if (0 >= nResult)
    {
        return CA_ERR_OBJ_NAME_TOO_LONG;
    }

    pszStrPos = pszNameBuf + lstrlen(pszPre);
    for (; '\0' != pszStrPos[0]; pszStrPos++)
    {
        if (':' == pszStrPos[0] || '\\' == pszStrPos[0] ||
            '/' == pszStrPos[0])
        {
            pszStrPos[0] = '_';
        }
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_GetCurTimeStr(TCHAR *pszTmBuf, DWORD dwBufCnt)
{
    struct tm *pTm;
    CAErrno funcErr = CA_ERR_SUCCESS;
    time_t tmNow;
    int nResult;

    CA_RTCSEnter();
    tmNow = time(NULL);
    pTm = localtime(&tmNow);
    if (NULL == pTm)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    nResult = CA_SNPrintf(pszTmBuf, dwBufCnt, 
        TEXT("%04u-%02u-%02u %02u:%02u:%02u"), 
        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, 
        pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    if (0 >= nResult)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

EXIT:
    CA_RTCSLeave();
    return funcErr;
}

CA_DECLARE(CAErrno) CA_GetTimeStr(time_t tmVal, TCHAR *pszTmBuf, 
                                  DWORD dwBufCnt)
{
    struct tm *pTm;
    CAErrno funcErr = CA_ERR_SUCCESS;
    int nResult;

    CA_RTCSEnter();
    pTm = localtime(&tmVal);
    if (NULL == pTm)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    nResult = CA_SNPrintf(pszTmBuf, dwBufCnt, 
        TEXT("%04u-%02u-%02u %02u:%02u:%02u"), 
        pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday, 
        pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    if (0 >= nResult)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

EXIT:
    CA_RTCSLeave();
    return funcErr;
}

CA_DECLARE(CAErrno) CA_GetFSize(const TCHAR *pszFName, 
                                CAFSize *pFSize)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;

    pFSize->dwFSize = 0;
    pFSize->dwFSizeHigh = 0;
    hFind = FindFirstFile(pszFName, &findData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        return CA_ERR_FNAME_CANNOT_FIND;
    }
    FindClose(hFind);    

    pFSize->dwFSize = findData.nFileSizeLow;
    pFSize->dwFSizeHigh = findData.nFileSizeHigh;
    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CA_SetFSize(CAFSize *pFSize, 
                             DWORD dwFSize, DWORD dwFSizeHigh)
{
    pFSize->dwFSize = dwFSize;
    pFSize->dwFSizeHigh = dwFSizeHigh;
}

CA_DECLARE(void) CA_AddFSize(CAFSize *pFSize, 
                             DWORD dwIncreaseFSize)
{
    /* FIXME use int64 to calculate the file size */
    pFSize->dwFSize += dwIncreaseFSize;
}

CA_DECLARE(CAErrno) CA_MkSubDirWithBName(const TCHAR *pszDir, 
                                         TCHAR *pszRealNameBuf, 
                                         DWORD dwNameBufCnt)
{
    WIN32_FIND_DATA findData;
    const TCHAR *pszRealName = pszDir;
    CAErrno funcErr = CA_ERR_SUCCESS;
    HANDLE hFind;
    TCHAR szReName[MAX_PATH];
    BOOL bResult;
    int nResult;
    int nRNameId;

    hFind = FindFirstFile(pszDir, &findData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        bResult = CreateDirectory(pszDir, NULL);
        if (!bResult)
        {
            funcErr = CA_ERR_FCREATE;
            goto EXIT;
        }

        goto EXIT;
    }
    FindClose(hFind);
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        goto EXIT;
    }
    
    /* XXX fixme set max retry count */
    for (nRNameId = 1;; nRNameId++)
    {
        nResult = CA_SNPrintf(szReName, sizeof(szReName) / sizeof(szReName[0]), 
            TEXT("%s_%u"), pszDir, nRNameId);
        if (0 >= nResult)
        {
            funcErr = CA_ERR_FNAME_TOO_LONG;
            break;
        }
        pszRealName = szReName;

        hFind = FindFirstFile(pszDir, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            bResult = CreateDirectory(pszDir, NULL);
            if (!bResult)
            {
                funcErr = CA_ERR_FCREATE;
                break;
            }
        }
        FindClose(hFind);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            break;
        }
    }

EXIT:
    if (CA_ERR_SUCCESS == funcErr)
    {
        /* copy real name to result buf */
        nResult = CA_SNPrintf(pszRealNameBuf, dwNameBufCnt, 
            TEXT("%s"), pszRealName);
        if (0 >= nResult)
        {
            funcErr = CA_ERR_FNAME_TOO_LONG;
        }
    }

    return funcErr;
}

CA_DECLARE(CAErrno) CA_ConvertFNameStr(const TCHAR *pszSrc, 
                        TCHAR *pszFNameBuf, DWORD dwFNameBufCnt)
{
    TCHAR *pStrPos;
    int nResult;

    nResult = CA_SNPrintf(pszFNameBuf, dwFNameBufCnt, TEXT("%s"), pszSrc);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    for (pStrPos = pszFNameBuf; '\0' != pszFNameBuf[0]; pszFNameBuf++)
    {
        if ('\\' == pStrPos[0] || '/' == pStrPos[0] ||
            '?'  == pStrPos[0] || '*' == pStrPos[0] ||
            ':'  == pStrPos[0] || '%' == pStrPos[0] ||
            '<'  == pStrPos[0] || '>' == pStrPos[0] ||
            '"'  == pStrPos[0] || '|' == pStrPos[0])
        {
            pStrPos[0] = '_';
        }
    }

    return CA_ERR_SUCCESS;
}
