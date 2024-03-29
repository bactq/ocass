/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 *
 */

#include "ca_chr.h"
#include "ca_mm.h"
#include "ca_str.h"
#include "ca_misc.h"
#include "ca_cfg.h"
#include "ca_xf.h"
#include "ca_inner.h"

#define CA_CHR_REC_ITEM_MAX_GUEST_LEN   (512)

typedef struct _CA_CHR_REC_Item
{
    time_t tmOpen;
    time_t tmLastUpdate;
    char szGuest[CA_CHR_REC_ITEM_MAX_GUEST_LEN];
    CAXF *pXF;

    struct _CA_CHR_REC_Item *pNext;
} CACHRRecItem;

struct _CA_CHART_HISTORY_REC
{
    CRITICAL_SECTION chrCS;

    TCHAR szCHRPath[MAX_PATH];
    TCHAR szTemplatePath[MAX_PATH];
    DWORD nMaxCacheSec;
    CACHRRecItem *pRecItemHdr;
};

static CAErrno CA_CHRecItemAppend(CACHRec *pCHR, CACHRRecItem *pRecItem, 
                                  CACHRItem *pAddData, DWORD dwRetryCnt)
{
    CAXFNode xfNode;
    CAErrno caErr;
    TCHAR szTmDesc[CA_TM_STR_MAX_BUF];

    if (NULL == pRecItem || NULL == pRecItem->pXF || NULL == pAddData)
    {
         CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Save chat item failed. bad args"));
        return CA_ERR_BAD_ARG;
    }

    caErr = CA_GetTimeStr(pAddData->tmAppend, szTmDesc, 
        sizeof(szTmDesc) / sizeof(szTmDesc[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
         CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Save chat item failed. bad time string"));
        return caErr;
    }

    pRecItem->tmLastUpdate = time(NULL); 
    xfNode.pszTm = szTmDesc;
    xfNode.dwCSeq = pAddData->dwCSeq;
    xfNode.pszCallId = pAddData->pszCallId;
    xfNode.pszFrom = pAddData->pszFrom;
    xfNode.pszTo = pAddData->pszTo;
    xfNode.pszMsg = pAddData->pszMsgData;
    xfNode.pwszMsg = pAddData->pwszMsgData;
    caErr = CA_XFAddNode(pRecItem->pXF, &xfNode);
    if (CA_ERR_SUCCESS != caErr)
    {
         CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Add node to xml file failed. err %u"), 
            caErr);
        return caErr;
    }

    caErr = CA_XFSave(pRecItem->pXF); 
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Add node to xml file failed. can't save xml file. err %u"), 
            caErr);
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

static CACHRRecItem* CA_CHRecItemFindByGuest(CACHRec *pCHR, 
                                             const char *pszGuest)
{
    CACHRRecItem *pFindItem = NULL;
    int nResult;

    if (NULL == pszGuest || '\0' == pszGuest[0])
    {
        return NULL;
    }

    for (pFindItem = pCHR->pRecItemHdr; NULL != pFindItem; 
    pFindItem = pFindItem->pNext)
    {
        nResult = stricmp(pFindItem->szGuest, pszGuest);
        if (0 == nResult)
        {
            break;
        }
    }

    return pFindItem;
}

static CACHRRecItem* CA_CHRecItemCreate(CACHRec *pCHR, CACHRItem *pCHRItem, 
                                        const TCHAR *pszMasterDir)
{
    WIN32_FIND_DATA findData;
    CACHRRecItem *pCreateItem = NULL;
    const DWORD dwMaxXFSize = 1024 * 888;
    CAErrno caErr;
    HANDLE hFind;
    time_t tmOpen;
    TCHAR szRecXFNamePrefix[MAX_PATH];
    TCHAR szGuestFName[MAX_PATH];
    TCHAR szRecXFName[MAX_PATH];
    DWORD dwId;
    CAXF *pXF = NULL;
    char szGuest[CA_CHR_REC_ITEM_MAX_GUEST_LEN];
    int nResult;

    nResult = CA_SNPrintf(szGuest, sizeof(szGuest) / sizeof(szGuest[0]), 
        TEXT("%s"), pCHRItem->pszGuest);
    if (0 >= nResult)
    {
        /* guest name is bad */
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xf node failed, guest name (%s) is too long"), 
            pCHRItem->pszGuest);
        return NULL;
    }

    caErr = CA_ConvertFNameStr(pCHRItem->pszGuest, 
        szGuestFName, sizeof(szGuestFName) / sizeof(szGuestFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xf node failed, guest name (%s) is bad"), 
            pCHRItem->pszGuest);
        return NULL;
    }

    caErr = CA_PathJoin(pszMasterDir, szGuestFName, szRecXFNamePrefix, 
        sizeof(szRecXFNamePrefix) / sizeof(szRecXFNamePrefix[0]));    
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xf node failed. Guest file name is bad."
            "Base dir %s, guest file suffix %s"), 
            pszMasterDir, szGuestFName);

        return NULL;
    }

    for (dwId = 0;; dwId++)
    {
        if (0 == dwId)
        {
            nResult = CA_SNPrintf(szRecXFName, 
                sizeof(szRecXFName) / sizeof(szRecXFName[0]), 
                TEXT("%s.xml"), szRecXFNamePrefix);
        }
        else
        {
            nResult = CA_SNPrintf(szRecXFName, 
                sizeof(szRecXFName) / sizeof(szRecXFName[0]), 
                TEXT("%s_%u.xml"), szRecXFNamePrefix, dwId);
        }
        if (0 >= nResult)
        {
            /* file name to long */
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create xf node failed. Guest file name is bad."
                "File name prefix %s"), szRecXFNamePrefix);
            return NULL;
        }

        hFind = FindFirstFile(szRecXFName, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            caErr = CA_XFOpen(szRecXFName, TRUE, &pXF);
            if (CA_ERR_SUCCESS != caErr)
            {
                /* create failed */
                CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                    TEXT("Create xf node failed. "
                    "can't create xml file (%s). err %u"),
                    szRecXFName, caErr);
                return NULL;
            }

            break;
        }
        FindClose(hFind);

        if (FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes)
        {
            continue;
        }

        if (dwMaxXFSize <= findData.nFileSizeLow)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN, 
                TEXT("xml file (%s) size is too big "), 
                szRecXFName);
            continue;
        }

        caErr = CA_XFOpen(szRecXFName, FALSE, &pXF);
        if (CA_ERR_SUCCESS == caErr)
        {
            break;
        }
        else
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN, 
                TEXT("open xf node failed. "
                "can't create xml file (%s). err %u"),
                szRecXFName, caErr);
        }
    }
    if (NULL == pXF)
    {
        /* can't open xml file */
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("open xf node failed. can't create/load xml file"));
        return NULL;
    }

    pCreateItem = (CACHRRecItem*)CA_MAlloc(sizeof(CACHRRecItem));
    if (NULL == pCreateItem)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("open xf node failed. no memory"));

        /* no mem */
        CA_XFClose(pXF);
        return NULL;
    }

    tmOpen = time(NULL);
    pCreateItem->pNext = NULL;
    strcpy(pCreateItem->szGuest, szGuest);
    pCreateItem->pXF = pXF;
    pCreateItem->tmOpen = tmOpen;
    pCreateItem->tmLastUpdate = tmOpen;

    pCreateItem->pNext = pCHR->pRecItemHdr;
    pCHR->pRecItemHdr = pCreateItem;
    return pCreateItem;
}

static BOOL CA_CHRecItemIsTimeOut(CACHRec *pCHR, time_t tmNow, 
                                  CACHRRecItem *pRecItem)
{
    time_t tmDiff = tmNow - pRecItem->tmLastUpdate;
    return (pCHR->nMaxCacheSec > (DWORD)tmDiff ? FALSE : TRUE);
}

static CAErrno CA_CHRecCloseItem(CACHRec *pCHR, 
                                 CACHRRecItem *pRecItem)
{
    if (NULL == pRecItem)
    {
        return CA_ERR_BAD_ARG;
    }

    if (NULL != pRecItem->pXF)
    {
        CA_XFSave(pRecItem->pXF);
        CA_XFClose(pRecItem->pXF);
    }

    CA_MFree(pRecItem);
    return CA_ERR_SUCCESS;
}

static CAErrno CA_CHRecCloseAll(CACHRec *pCHR)
{
    CACHRRecItem *pRecItem;

    for (pRecItem = pCHR->pRecItemHdr; NULL != pCHR->pRecItemHdr;)
    {
        pCHR->pRecItemHdr = pRecItem->pNext;
        CA_CHRecCloseItem(pCHR, pRecItem);
        pRecItem = pCHR->pRecItemHdr;
    }
    
    return CA_ERR_SUCCESS;
}

static CAErrno CA_CHRecCloseTimeOut(CACHRec *pCHR)
{
    CACHRRecItem *pPreRecItem;
    CACHRRecItem *pRecItem;
    CACHRRecItem *pRmRecItem;
    time_t tmNow = time(NULL);
    BOOL bResult;

    for (pPreRecItem = NULL, pRecItem = pCHR->pRecItemHdr; NULL != pRecItem;)
    {
        bResult = CA_CHRecItemIsTimeOut(pCHR, tmNow, pRecItem);
        if (!bResult)
        {
            pPreRecItem = pRecItem;
            pRecItem = pRecItem->pNext;
            continue;
        }

        pRmRecItem = pRecItem;
        if (NULL == pPreRecItem)
        {
            pRecItem = pRecItem->pNext;
            pCHR->pRecItemHdr = pRecItem;
        }
        else
        {
            pPreRecItem->pNext = pRecItem->pNext;
            pRecItem = pRecItem->pNext;
        }        

        CA_CHRecCloseItem(pCHR, pRmRecItem);
    }

    return CA_ERR_SUCCESS;
}

static CAErrno CA_CHRCreateMasterDir(CACHRec *pCHR, CACHRItem *pCHRItem, 
                    TCHAR *pszMDirBuf, DWORD dwMDirBufCnt)
{
    CAErrno caErr;
    TCHAR szMasterSuffix[MAX_PATH];
    TCHAR szMasterDir[MAX_PATH];
    TCHAR szRealMasterDir[MAX_PATH];
    int nResult;

    caErr = CA_ConvertFNameStr(pCHRItem->pszMaster, 
        szMasterSuffix, sizeof(szMasterSuffix) / sizeof(szMasterSuffix[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create master dir failed, bad master name %s"), 
            pCHRItem->pszMaster);

        return caErr;
    }

    /* create master save dir */
    caErr = CA_PathJoin(pCHR->szCHRPath, szMasterSuffix, 
        szMasterDir, sizeof(szMasterDir) / sizeof(szMasterDir[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create master dir failed, bad master name %s"), 
            pCHRItem->pszMaster);

        return caErr;
    }

    caErr = CA_MkSubDirWithBName(szMasterDir, szRealMasterDir, 
        sizeof(szRealMasterDir) / sizeof(szRealMasterDir[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create master dir failed, bad master name %s"), 
            pCHRItem->pszMaster);

        return caErr;
    }

    nResult = CA_SNPrintf(pszMDirBuf, dwMDirBufCnt, TEXT("%s"), 
        szRealMasterDir);
    if (0 >= nResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create master dir failed, path name (%s) is too long"), 
            szRealMasterDir);
    }
    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

static CAErrno CA_CHRWrGuestRec(CACHRec *pCHR, CACHRItem *pCHRItem, 
                                const TCHAR *pszMasterDir, DWORD dwRetryCnt)
{
    CACHRRecItem *pGuestNode;

    pGuestNode = CA_CHRecItemFindByGuest(pCHR, pCHRItem->pszGuest);
    if (NULL != pGuestNode)
    {
        return CA_CHRecItemAppend(pCHR, pGuestNode, pCHRItem, dwRetryCnt);
    }

    /* create new guset node */
    pGuestNode = CA_CHRecItemCreate(pCHR, pCHRItem, pszMasterDir);
    if (NULL == pGuestNode)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xf item node failed"));
        return CA_ERR_FCREATE;
    }

    return CA_CHRecItemAppend(pCHR, pGuestNode, pCHRItem, dwRetryCnt);
}

CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR, DWORD nMaxCacheSec)
{
    CRITICAL_SECTION *pCHRCS = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CACHRec *pNewCHR = NULL;
    int nResult;

    pNewCHR = (CACHRec *)CA_MAlloc(sizeof(CACHRec));
    if (NULL == pNewCHR)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("create CHR failed. no mem"));
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    CA_RTCSEnter();
    nResult = CA_SNPrintf(pNewCHR->szCHRPath, 
        sizeof(pNewCHR->szCHRPath) / sizeof(pNewCHR->szCHRPath[0]), 
        TEXT("%s"), CA_CfgGetRT()->szHistoryPath);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("create CHR failed. history path too long"));
        funcErr = CA_ERR_FNAME_TOO_LONG;
        goto EXIT;
    }

    CA_RTCSEnter();
    nResult = CA_SNPrintf(pNewCHR->szTemplatePath, 
        sizeof(pNewCHR->szTemplatePath) / sizeof(pNewCHR->szTemplatePath[0]), 
        TEXT("%s"), CA_CfgGetRT()->szTemplatePath);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("create CHR failed. template path too long"));
        funcErr = CA_ERR_FNAME_TOO_LONG;
        goto EXIT;
    }

    InitializeCriticalSection(&pNewCHR->chrCS);
    pCHRCS = &pNewCHR->chrCS;
    pNewCHR->nMaxCacheSec = nMaxCacheSec;
    pNewCHR->pRecItemHdr = NULL;

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pCHRCS)
        {
            DeleteCriticalSection(pCHRCS);
        }

        if (NULL != pNewCHR)
        {
            CA_MFree(pNewCHR);
        }
    }
    else
    {
        *pCHR = pNewCHR;
    }

    return funcErr;
}

CA_DECLARE(CAErrno) CA_CHRecClose(CACHRec *pCHR)
{
    if (NULL == pCHR)
    {
        return CA_ERR_BAD_ARG;
    }

    EnterCriticalSection(&pCHR->chrCS);
    CA_CHRecCloseAll(pCHR);
    LeaveCriticalSection(&pCHR->chrCS);
    DeleteCriticalSection(&pCHR->chrCS);
    CA_MFree(pCHR);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR, 
                                      const TCHAR *pszHistoryPath)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    TCHAR szCHRPath[MAX_PATH];
    TCHAR szCHStylePath[MAX_PATH];
    BOOL bNeedClose;
    int nResult;

    nResult = CA_SNPrintf(szCHRPath, 
        sizeof(szCHRPath) / sizeof(szCHRPath[0]), TEXT("%s"), 
        pszHistoryPath);
    if (0 >= nResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("update config failed. hsitory path is too long"));
        return CA_ERR_FNAME_TOO_LONG;
    }

    CA_RTCSEnter();
    nResult = CA_SNPrintf(szCHStylePath, 
        sizeof(szCHStylePath) / sizeof(szCHStylePath[0]), TEXT("%s"), 
        CA_CfgGetRT()->szTemplatePath);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("update config failed. template path is too long"));
        return CA_ERR_FNAME_TOO_LONG;
    }

    bNeedClose = FALSE;
    EnterCriticalSection(&pCHR->chrCS);
    nResult = lstrcmpi(pCHR->szCHRPath, szCHRPath);
    if (0 != nResult)
    {
        bNeedClose = TRUE;
        lstrcpy(pCHR->szCHRPath, szCHRPath);
    }

    nResult = lstrcmpi(pCHR->szTemplatePath, szCHStylePath);
    if (0 != nResult)
    {
        bNeedClose = TRUE;
        lstrcpy(pCHR->szTemplatePath, szCHStylePath);
    }

    if (bNeedClose)
    {
        funcErr = CA_CHRecCloseAll(pCHR);
    }
    LeaveCriticalSection(&pCHR->chrCS);
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CHRecAppend(CACHRec *pCHR, DWORD dwRetryCnt, 
                                   CACHRItem *pCHRItem)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    TCHAR szMasterDir[MAX_PATH];

    caErr = CA_CHRCreateMasterDir(pCHR, pCHRItem, szMasterDir, 
        sizeof(szMasterDir) / sizeof(szMasterDir[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create master dir failed. err %u"), caErr);
        return caErr;
    }

    /* copy style file to master dir */
    CA_CHRCpStyleFile(pCHR, szMasterDir);

    EnterCriticalSection(&pCHR->chrCS);
    funcErr = CA_CHRWrGuestRec(pCHR, pCHRItem, szMasterDir, dwRetryCnt);
    LeaveCriticalSection(&pCHR->chrCS);
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CHRCpStyleFile(CACHRec *pCHR, 
                                      const TCHAR *pszDestPath)
{
    WIN32_FIND_DATA findData;
    CAErrno caErr;
    HANDLE hFind;
    TCHAR szStyleSrcFName[MAX_PATH];
    TCHAR szStyleDestFName[MAX_PATH];
    BOOL bResult;

    caErr = CA_PathJoin(pszDestPath, CA_XF_XSL_FNAME, szStyleDestFName, 
        sizeof(szStyleDestFName) / sizeof(szStyleDestFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        /* file name to long */
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("copy style file failed. dest file name too long"
            "base dir (%s). file name (%s)"), 
            pszDestPath, CA_XF_XSL_FNAME);
        return caErr;
    }

    EnterCriticalSection(&pCHR->chrCS);
    caErr = CA_PathJoin(pCHR->szTemplatePath, CA_XF_XSL_FNAME, szStyleSrcFName, 
        sizeof(szStyleSrcFName) / sizeof(szStyleSrcFName[0]));
    LeaveCriticalSection(&pCHR->chrCS);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("copy style file failed. source file name too long"));
        return caErr;
    }

    hFind = FindFirstFile(szStyleDestFName, &findData);
    if (INVALID_HANDLE_VALUE != hFind)
    {
        FindClose(hFind);
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            return CA_ERR_FCREATE;
        }

        return CA_ERR_SUCCESS;
    }

    /* copy file */
    bResult = CopyFile(szStyleSrcFName, szStyleDestFName, TRUE);
    if (!bResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR|CA_RTLOG_OSERR, 
            TEXT("copy style file failed. source %s dest %s"), 
            szStyleSrcFName, szStyleDestFName);
        return CA_ERR_FCREATE;
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecCloseTimeOutSlot(CACHRec *pCHR)
{
    CAErrno caErr;

    EnterCriticalSection(&pCHR->chrCS);
    caErr = CA_CHRecCloseTimeOut(pCHR);
    LeaveCriticalSection(&pCHR->chrCS);
    return caErr;
}

CA_DECLARE(CAErrno) CA_CHRecCloseAllSlot(CACHRec *pCHR)
{
    CAErrno caErr;

    EnterCriticalSection(&pCHR->chrCS);
    caErr = CA_CHRecCloseAll(pCHR);
    LeaveCriticalSection(&pCHR->chrCS);
    return caErr;
}
