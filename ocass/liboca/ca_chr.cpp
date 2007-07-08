/**
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
        return CA_ERR_BAD_ARG;
    }

    caErr = CA_GetTimeStr(pAddData->tmAppend, szTmDesc, 
        sizeof(szTmDesc) / sizeof(szTmDesc[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pRecItem->tmLastUpdate = time(NULL); 
    xfNode.pszTm = szTmDesc;
    xfNode.dwCSeq = pAddData->dwCSeq;
    xfNode.pszCallId = pAddData->pszCallId;
    xfNode.pszFrom = pAddData->pszFrom;
    xfNode.pszTo = pAddData->pszTo;
    xfNode.pszMsg = pAddData->pszMsg;
    caErr = CA_XFAddNode(pRecItem->pXF, &xfNode);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_XFSave(pRecItem->pXF); 
    if (CA_ERR_SUCCESS != caErr)
    {
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
        return NULL;
    }

    caErr = CA_ConvertFNameStr(pCHRItem->pszGuest, 
        szGuestFName, sizeof(szGuestFName) / sizeof(szGuestFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return NULL;
    }

    caErr = CA_PathJoin(pszMasterDir, szGuestFName, szRecXFNamePrefix, 
        sizeof(szRecXFNamePrefix) / sizeof(szRecXFNamePrefix[0]));    
    if (CA_ERR_SUCCESS != caErr)
    {
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
            return NULL;
        }

        hFind = FindFirstFile(szRecXFName, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            caErr = CA_XFOpen(szRecXFName, TRUE, &pXF);
            if (CA_ERR_SUCCESS != caErr)
            {
                /* create failed */
                return NULL;
            }
            break;
        }
        FindClose(hFind);

        if (FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes)
        {
            continue;
        }

        caErr = CA_XFOpen(szRecXFName, FALSE, &pXF);
        if (CA_ERR_SUCCESS == caErr)
        {
            break;
        }
    }
    if (NULL == pXF)
    {
        /* can't open xml file */
        return NULL;
    }

    pCreateItem = (CACHRRecItem*)CA_MAlloc(sizeof(CACHRRecItem));
    if (NULL == pCreateItem)
    {
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
        return caErr;
    }

    /* create master save dir */
    caErr = CA_PathJoin(pCHR->szCHRPath, szMasterSuffix, 
        szMasterDir, sizeof(szMasterDir) / sizeof(szMasterDir[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_MkSubDirWithBName(szMasterDir, szRealMasterDir, 
        sizeof(szRealMasterDir) / sizeof(szRealMasterDir[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    nResult = CA_SNPrintf(pszMDirBuf, dwMDirBufCnt, TEXT("%s"), 
        szRealMasterDir);
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

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    TCHAR szCHRPath[MAX_PATH];
    TCHAR szCHStylePath[MAX_PATH];
    int nResult;

    CA_RTCSEnter();
    nResult = CA_SNPrintf(szCHRPath, 
        sizeof(szCHRPath) / sizeof(szCHRPath[0]), TEXT("%s"), 
        CA_CfgGetRT()->szHistoryPath);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    CA_RTCSEnter();
    nResult = CA_SNPrintf(szCHStylePath, 
        sizeof(szCHStylePath) / sizeof(szCHStylePath[0]), TEXT("%s"), 
        CA_CfgGetRT()->szTemplatePath);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    /* FIXME read cache time from config or args */
    EnterCriticalSection(&pCHR->chrCS);
    lstrcpy(pCHR->szCHRPath, szCHRPath);
    lstrcpy(pCHR->szTemplatePath, szCHStylePath);
    funcErr = CA_CHRecCloseAll(pCHR);
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
        return caErr;
    }

    EnterCriticalSection(&pCHR->chrCS);
    caErr = CA_PathJoin(pCHR->szTemplatePath, CA_XF_XSL_FNAME, szStyleSrcFName, 
        sizeof(szStyleSrcFName) / sizeof(szStyleSrcFName[0]));
    LeaveCriticalSection(&pCHR->chrCS);
    if (CA_ERR_SUCCESS != caErr)
    {
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
        return CA_ERR_FCREATE;
    }

    return CA_ERR_SUCCESS;
}
