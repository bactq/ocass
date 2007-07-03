/**
 *
 */

#include "ca_chr.h"
#include "ca_mm.h"
#include "ca_str.h"
#include "ca_misc.h"
#include "ca_cfg.h"
#include "ca_inner.h"

#define CA_CHR_MAX_REC_SLOTS_CNT     (300)

typedef struct _CA_CHR_REC_FD_SLOT
{
    BOOL bIsUsed;

    FILE    *pFile;
    TCHAR   szFName[MAX_PATH];
} CACHRRecFdSlot;

struct _CA_CHART_HISTORY_REC
{
    CRITICAL_SECTION chrCS;

    TCHAR szCHRPath[MAX_PATH];
    CACHRRecFdSlot recSlots[CA_CHR_MAX_REC_SLOTS_CNT];
};

static CAErrno CA_CHRecWrHdr(CACHRRecFdSlot *pSlot, DWORD dwRetryCnt, 
                             CACHRItem *pCHRItem, BOOL bNeedFlush)
{
    if (NULL == pSlot || NULL == pSlot->pFile)
    {
        return CA_ERR_FOPEN;
    }

    /* XXX FIXME set utf8 version to xml */
    fprintf(pSlot->pFile, "<?xml version=\"1.0\"?>");
    if (bNeedFlush)
    {
        fflush(pSlot->pFile);
    }
    return CA_ERR_SUCCESS;
}

static CAErrno CA_CHRecWrItem(CACHRRecFdSlot *pSlot, DWORD dwRetryCnt, 
                              CACHRItem *pCHRItem, BOOL bNeedFlush)
{
    TCHAR szTmStamp[128] = {0};

    if (NULL == pSlot || NULL == pSlot->pFile)
    {
        return CA_ERR_FOPEN;
    }

    /* XXX FIXME we need convert all xml key string */
    fprintf(pSlot->pFile, 
            "\n"
            "<MsgItem TimeStamp=\"%s\"> \n"
            "   <MsgCallId CSeq=\"%u\">%s</MsgCallId> \n"
            "   <MsgFrom>%s</MsgFrom> \n"
            "   <MsgTo>%s</MsgTo> \n"
            "   <MsgData>%s</MsgData> \n"
            "</MsgItem>\n"
            "\n", 
            szTmStamp, pCHRItem->dwCSeq, pCHRItem->pszCallId, 
            pCHRItem->pszMsg, pCHRItem->pszFrom, pCHRItem->pszTo);

    if (bNeedFlush)
    {
        fflush(pSlot->pFile);
    }
    return CA_ERR_SUCCESS;
}

static CACHRRecFdSlot* CA_CHRGetFirstFreeSlot(CACHRec *pCHR)
{
    CACHRRecFdSlot *pFind = NULL;
    CACHRRecFdSlot *pPos;
    DWORD dwSlotsCnt;
    DWORD dwSlotId;

    dwSlotsCnt = sizeof(pCHR->recSlots) / sizeof(pCHR->recSlots[0]);
    for (dwSlotId = 0; dwSlotId < dwSlotsCnt; dwSlotId++)
    {
        pPos = &pCHR->recSlots[dwSlotId];
        if (pPos->bIsUsed)
        {
            continue;
        }

        pFind = pPos;
        break;
    }

    return pFind;
}

static CACHRRecFdSlot* CA_CHRFindSlotByFName(CACHRec *pCHR, 
                                             const TCHAR *pszFName)
{
    CACHRRecFdSlot *pFind = NULL;
    CACHRRecFdSlot *pPos;
    DWORD dwSlotsCnt;
    DWORD dwSlotId;
    int nResult;

    if (NULL == pszFName || '\0' == pszFName[0])
    {
        return NULL;
    }

    dwSlotsCnt = sizeof(pCHR->recSlots) / sizeof(pCHR->recSlots[0]);
    for (dwSlotId = 0; dwSlotId < dwSlotsCnt; dwSlotId++)
    {
        pPos = &pCHR->recSlots[dwSlotId];
        if (!pPos->bIsUsed)
        {
            break;
        }

        nResult = lstrcmpi(pszFName, pPos->szFName);
        if (0 == nResult)
        {
            pFind = pPos;
            break;
        }
    }

    return pFind;
}

static CAErrno CA_CHRecFlushAllSlots(CACHRec *pCHR, BOOL bNeedClose)
{
    CACHRRecFdSlot *pPos;
    DWORD dwSlotsCnt;
    DWORD dwSlotId;

    dwSlotsCnt = sizeof(pCHR->recSlots) / sizeof(pCHR->recSlots[0]);
    for (dwSlotId = 0; dwSlotId < dwSlotsCnt; dwSlotId++)
    {
        pPos = &pCHR->recSlots[dwSlotId];
        if (!pPos->bIsUsed)
        {
            break;
        }

        if (NULL != pPos->pFile)
        {
            fflush(pPos->pFile);
        }

        if (!bNeedClose)
        {
            continue;
        }

        if (NULL != pPos->pFile)
        {
            fclose(pPos->pFile);
            pPos->pFile = NULL;
        }

        pPos->szFName[0] = '\0';
        pPos->bIsUsed = FALSE;
    }

    return CA_ERR_SUCCESS;
}

static CAErrno CA_CHRecWrite(CACHRec *pCHR, CACHRRecFdSlot *pSlot, 
                             const TCHAR *pszFName, DWORD dwRetryCnt, 
                             CACHRItem *pCHRItem)
{
    CAErrno caErr;
    CAFSize fileSize;
    int nResult;

    if (pSlot->bIsUsed)
    {
        return CA_CHRecWrItem(pSlot, dwRetryCnt, pCHRItem, TRUE);
    }

    nResult = CA_SNPrintf(pSlot->szFName, 
        sizeof(pSlot->szFName) / sizeof(pSlot->szFName[0]), 
        TEXT("%s"), pszFName);
    if (0 >= nResult)
    {
        pSlot->szFName[0] =  '\0';
        return CA_ERR_FNAME_TOO_LONG;
    }

    caErr = CA_GetFSize(pSlot->szFName, &fileSize);
    if (CA_ERR_SUCCESS != caErr)
    {
        fileSize.dwFSize = 0;
        fileSize.dwFSizeHigh = 0;
    }

    pSlot->pFile = fopen(pSlot->szFName, "a+");
    if (NULL == pSlot->pFile)
    {
        pSlot->szFName[0] = '\0';
        return CA_ERR_FOPEN;
    }
    pSlot->bIsUsed = TRUE;
    
    if (0 == fileSize.dwFSize)
    {
        CA_CHRecWrHdr(pSlot, dwRetryCnt, pCHRItem, FALSE);
    }

    return CA_CHRecWrItem(pSlot, dwRetryCnt, pCHRItem, TRUE);
}

CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR)
{
    CRITICAL_SECTION *pCHRCS = NULL;
    CACHRec *pNewCHR = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    DWORD dwSlotsCnt;
    DWORD dwSlotId;
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

    InitializeCriticalSection(&pNewCHR->chrCS);
    pCHRCS = &pNewCHR->chrCS;

    /* init slots mem */
    dwSlotsCnt = sizeof(pNewCHR->recSlots) / sizeof(pNewCHR->recSlots[0]);
    for (dwSlotId = 0; dwSlotId < dwSlotsCnt; dwSlotId++)
    {
        (pNewCHR->recSlots[dwSlotId]).bIsUsed = FALSE;
        (pNewCHR->recSlots[dwSlotId]).pFile = NULL;
        (pNewCHR->recSlots[dwSlotId]).szFName[0] = '\0';
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        /* free all mem and handle */
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
        return CA_ERR_SUCCESS;
    }

    CA_CHRecFlushAll(pCHR, TRUE);
    DeleteCriticalSection(&pCHR->chrCS);
    CA_MFree(pCHR);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    TCHAR szCHRPath[MAX_PATH];
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

    EnterCriticalSection(&pCHR->chrCS);
    lstrcpy(pCHR->szCHRPath, szCHRPath);
    funcErr = CA_CHRecFlushAllSlots(pCHR, TRUE);
    LeaveCriticalSection(&pCHR->chrCS);
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CHRecFlushAll(CACHRec *pCHR, 
                                     BOOL bNeedClose)
{
    CAErrno caErr;

    EnterCriticalSection(&pCHR->chrCS);
    caErr = CA_CHRecFlushAllSlots(pCHR, bNeedClose);
    LeaveCriticalSection(&pCHR->chrCS);
    return caErr;
}

CA_DECLARE(CAErrno) CA_CHRecAppend(CACHRec *pCHR, DWORD dwRetryCnt, 
                                   CACHRItem *pCHRItem)
{
    CACHRRecFdSlot *pCHRSlot;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    TCHAR szCHRFName[MAX_PATH];
    DWORD dwRealRetryCnt;
    BOOL bIsNewSlot;

    caErr = CA_CHRecNaming(pCHR, pCHRItem, szCHRFName, 
        sizeof(szCHRFName) / sizeof(szCHRFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {        
        return caErr;
    }

    EnterCriticalSection(&pCHR->chrCS);
    bIsNewSlot = FALSE;
    pCHRSlot = CA_CHRFindSlotByFName(pCHR, szCHRFName);
    if (NULL == pCHRSlot)
    {
        bIsNewSlot = TRUE;
        pCHRSlot = CA_CHRGetFirstFreeSlot(pCHR);
        if (NULL == pCHRSlot)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
    }

    dwRealRetryCnt = (CA_CHR_MAX_RETRY <= dwRetryCnt ? 
            CA_CHR_MAX_RETRY : dwRetryCnt);
    funcErr = CA_CHRecWrite(pCHR, pCHRSlot, szCHRFName, 
        dwRealRetryCnt, pCHRItem);
EXIT:
    LeaveCriticalSection(&pCHR->chrCS);
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CHRecNaming(CACHRec *pCHR, CACHRItem *pCHRItem, 
                                   TCHAR *pszSFNameBuf, 
                                   DWORD dwSFNameBufCnt)
{
    WIN32_FIND_DATA findData;
    const int nMaxReNameCnt = 20;
    CAErrno caErr;
    HANDLE hFind;
    TCHAR szMasterDir[MAX_PATH];
    TCHAR szRealMasterDir[MAX_PATH];  
    TCHAR szGuestFName[MAX_PATH];
    TCHAR szRealGuestFName[MAX_PATH];
    TCHAR szGuestSuffix[MAX_PATH];
    TCHAR szMasterSuffix[MAX_PATH];
    BOOL bFind;
    int nReNameId;
    int nResult;

    caErr = CA_ConvertFNameStr(pCHRItem->pszMaster, 
        szMasterSuffix, sizeof(szMasterSuffix) / sizeof(szMasterSuffix[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_ConvertFNameStr(pCHRItem->pszGuest, 
        szGuestSuffix, sizeof(szGuestSuffix) / sizeof(szGuestSuffix[0]));
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

    /* naming guest file */
    caErr = CA_PathJoin(szRealMasterDir, szGuestSuffix, 
        szGuestFName, sizeof(szGuestFName) / sizeof(szGuestFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    for (nReNameId = 0, bFind = FALSE;
    nReNameId < nMaxReNameCnt; nReNameId++)
    {
        if (0 == nReNameId)
        {
            nResult = CA_SNPrintf(szRealGuestFName, 
                sizeof(szRealGuestFName) / sizeof(szRealGuestFName[0]), 
                TEXT("%s.xml"), szGuestFName);
        }
        else
        {
            nResult = CA_SNPrintf(szRealGuestFName, 
                sizeof(szRealGuestFName) / sizeof(szRealGuestFName[0]), 
                TEXT("%s_%u.xml"), szGuestFName, nReNameId);
        }
        if (0 >= nResult)
        {
            return CA_ERR_FNAME_TOO_LONG;
        }

        hFind = FindFirstFile(szRealGuestFName, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            bFind = TRUE;
            break;
        }
        FindClose(hFind);
        
    }
    if (!bFind)
    {
        return CA_ERR_FCREATE;
    }

    nResult = CA_SNPrintf(pszSFNameBuf, dwSFNameBufCnt, 
        TEXT("%s"), szRealGuestFName);
    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}
