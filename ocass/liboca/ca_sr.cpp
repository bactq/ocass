/**
 *
 */

#include "liboca.h"
#include "ca_sr.h"
#include "ca_str.h"
#include "ca_evts.h"
#include "ca_cfg.h"
#include "ca_misc.h"
#include "ca_flock.h"
#include "ca_mm.h"
#include "ca_inner.h"

struct _CA_SR
{
    BOOL bIsAttach;

    CAFLock spyFLock;
    CAShMM *pSHMM;
    CASpyDatum *pSpyDatum;
};

static CAErrno CA_SRDupFromCfg(CACfgDatum *pSrc, CASpyDatum *pDest)
{
    int nResult;

    pDest->szHistoryPath[0] = '\0';
    pDest->szSpyLog[0] = '\0';
    pDest->szSpyNtDump[0] = '\0';
    pDest->logMask = pSrc->spyLogMask;  
    pDest->spyState = CA_SPY_STATE_PREPARE;
    pDest->stateStartTime = time(NULL);
    pDest->bStateIsDirty = TRUE;

    nResult = CA_SNPrintf(pDest->szHistoryPath, 
        sizeof(pDest->szHistoryPath) / sizeof(pDest->szHistoryPath[0]), 
        TEXT("%s"), pSrc->szHistoryPath);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    nResult = CA_SNPrintf(pDest->szSpyLog, 
        sizeof(pDest->szSpyLog) / sizeof(pDest->szSpyLog[0]), 
        TEXT("%s"), pSrc->szSpyLog);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    nResult = CA_SNPrintf(pDest->szSpyNtDump, 
        sizeof(pDest->szSpyNtDump) / sizeof(pDest->szSpyNtDump[0]), 
        TEXT("%s"), pSrc->szSpyNtDump);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    return CA_ERR_SUCCESS;
}

static CAErrno CA_SRAutoDup(CASpyDatum *pDest)
{
    CAErrno caErr;

    CA_RTCSEnter();
    caErr = CA_SRDupFromCfg(CA_CfgGetRT(), pDest);
    CA_RTCSLeave();
    return caErr;
}

static CAErrno CA_SRGetShmmFName(TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    CAErrno caErr;

    CA_RTCSEnter();
    caErr = CA_PathJoin(CA_GetPtrRT()->szRTWrkPath, 
        CA_SPY_RUN_DATUM_FNAME, pszFNameBuf, dwBufCnt);
    CA_RTCSLeave();
    return caErr;
}

static CAErrno CA_SRGetLockFName(TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    CAErrno caErr;

    CA_RTCSEnter();
    caErr = CA_PathJoin(CA_GetPtrRT()->szRTWrkPath, 
        CA_SPY_RUN_LOCK_FNAME, pszFNameBuf, dwBufCnt);
    CA_RTCSLeave();
    return caErr;
}

CA_DECLARE(CAErrno) CA_SRCreate(CASpyRun **pSR, BOOL bAutoDupCfg)
{
    CASpyRun *pNewSR = NULL;
    CAFLock *pSpyFLock = NULL;
    CAErrno caErr;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAShMM *pSHMM = NULL;
    TCHAR szLockFName[MAX_PATH];
    TCHAR szShMMFName[MAX_PATH];

    caErr = CA_SRGetShmmFName(szShMMFName, 
        sizeof(szShMMFName) / sizeof(szShMMFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_SRGetLockFName(szLockFName, 
        sizeof(szLockFName) / sizeof(szLockFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pNewSR = (CASpyRun *)CA_MAlloc(sizeof(CASpyRun));
    if (NULL == pNewSR)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }
    memset(pNewSR, 0, sizeof(CASpyRun));

    /* create flock */
    caErr = CA_FLockCreate(szLockFName, &pNewSR->spyFLock);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pSpyFLock = &pNewSR->spyFLock;

    /* create datum */
    caErr = CA_ShMMCreate(sizeof(CASpyDatum), szShMMFName, 
        &pNewSR->pSHMM);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pSHMM = pNewSR->pSHMM;

    pNewSR->pSpyDatum = (CASpyDatum *)CA_ShMMBaseAddrGet(pNewSR->pSHMM);
    if (NULL == pNewSR->pSpyDatum ||
        sizeof(CASpyDatum) > CA_ShMMSizeGet(pNewSR->pSHMM))
    {
        funcErr = CA_ERR_FATAL;
        goto EXIT;
    }

    if (bAutoDupCfg)
    {
        caErr = CA_SRAutoDup(pNewSR->pSpyDatum);
        if (CA_ERR_SUCCESS != caErr)
        {
            funcErr = caErr;
            goto EXIT;
        }
    }

    pNewSR->bIsAttach = FALSE;    
EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        /* create failed */
        if (NULL != pSpyFLock)
        {
            CA_FLockDestroy(pSpyFLock, TRUE);
        }

        if (NULL != pSHMM)
        {
            CA_ShMMDestroy(pSHMM, TRUE);
        }

        if (NULL != pNewSR)
        {
            CA_MFree(pNewSR);
        }
    }
    else
    {
        *pSR = pNewSR;
    }

    return funcErr;
}

CA_DECLARE(CAErrno) CA_SRAttach(CASpyRun **pSR)
{
    CASpyRun *pNewSR = NULL;
    CAFLock *pSpyFLock = NULL;
    CAErrno caErr;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAShMM *pSHMM = NULL;
    TCHAR szLockFName[MAX_PATH];
    TCHAR szShMMFName[MAX_PATH];

    caErr = CA_SRGetShmmFName(szShMMFName, 
        sizeof(szShMMFName) / sizeof(szShMMFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_SRGetLockFName(szLockFName, 
        sizeof(szLockFName) / sizeof(szLockFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pNewSR = (CASpyRun *)CA_MAlloc(sizeof(CASpyRun));
    if (NULL == pNewSR)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }
    memset(pNewSR, 0, sizeof(CASpyRun));

    /* create flock */
    caErr = CA_FLockCreate(szLockFName, &pNewSR->spyFLock);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pSpyFLock = &pNewSR->spyFLock;

    /* attach datum */
    caErr = CA_ShMMAttach(szShMMFName, &pNewSR->pSHMM);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pSHMM = pNewSR->pSHMM;

    pNewSR->pSpyDatum = (CASpyDatum *)CA_ShMMBaseAddrGet(pNewSR->pSHMM);
    if (NULL == pNewSR->pSpyDatum ||
        sizeof(CASpyDatum) > CA_ShMMSizeGet(pNewSR->pSHMM))
    {
        funcErr = CA_ERR_FATAL;
        goto EXIT;
    }

    pNewSR->bIsAttach = FALSE;    
EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        /* create failed */
        if (NULL != pSpyFLock)
        {
            CA_FLockDestroy(pSpyFLock, TRUE);
        }

        if (NULL != pSHMM)
        {
            CA_ShMMDettach(pSHMM);
        }

        if (NULL != pNewSR)
        {
            CA_MFree(pNewSR);
        }
    }
    else
    {
        *pSR = pNewSR;
    }

    return funcErr;
}

CA_DECLARE(void) CA_SRClose(CASpyRun *pSR)
{
    if (NULL == pSR)
    {
        return;
    }

    if (pSR->bIsAttach)
    {
        CA_ShMMDettach(pSR->pSHMM);
    }
    else
    {
        CA_ShMMDestroy(pSR->pSHMM, TRUE);
    }

    CA_FLockDestroy(&pSR->spyFLock, TRUE);
    CA_MFree(pSR);
}

CA_DECLARE(CAErrno) CA_SRLock(CASpyRun *pSR, BOOL bWr)
{
    if (NULL == pSR)
    {
        return CA_ERR_BAD_ARG;
    }

    return CA_FLockEnter(&pSR->spyFLock, 
        bWr ? CA_FLOCK_OPT_RW : CA_FLOCK_OPT_RD);
}

CA_DECLARE(CAErrno) CA_SRUnlock(CASpyRun *pSR)
{
    if (NULL == pSR)
    {
        return CA_ERR_BAD_ARG;
    }

    return CA_FLockLeave(&pSR->spyFLock);
}

CA_DECLARE(CASpyDatum*) CA_SRGetDatum(CASpyRun *pSR)
{
    return pSR->pSpyDatum;
}

CA_DECLARE(CAErrno) CA_SRTouch(CASpyRun *pSR)
{
    HANDLE hEvt;
    BOOL bResult;

    hEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, OCASS_EVT_NAME_SPY_RUN);
    if (NULL == hEvt)
    {
        return CA_ERR_SYS_CALL;
    }

    bResult = SetEvent(hEvt);
    CloseHandle(hEvt);
    return (bResult ? CA_ERR_SUCCESS : CA_ERR_SYS_CALL);
}

CA_DECLARE(void) CA_SRUpdateState(CASpyRun *pSR, CASpyState spyState)
{
    CASpyDatum *pSD;

    CA_SRLock(pSR, TRUE);
    pSD = CA_SRGetDatum(pSR);
    if (pSD->spyState == spyState)
    {
        pSD->spyState = CA_SPY_STATE_PREPARE;
        pSD->stateStartTime = time(NULL);
        pSD->bStateIsDirty = TRUE;
    }
    CA_SRUnlock(pSR);
}

CA_DECLARE(void) CA_SRDatumDup(CASpyRun *pSR, CASpyDatum *pDup)
{
    CA_SRLock(pSR, FALSE);
    memcpy(pDup, pSR->pSpyDatum, sizeof(CASpyDatum));
    CA_SRUnlock(pSR);
}
