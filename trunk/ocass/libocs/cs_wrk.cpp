/**
 *
 */

#include "cs_wrk.h"
#include "ca_evts.h"
#include "ca_mm.h"
#include "ca_sr.h"

struct _CSWrk
{
    CRITICAL_SECTION wrkCS;
    HANDLE  hSpyRunEvt;
    HANDLE  hWrkTh;
    HANDLE  hWrkSem;
    BOOL    bStopWrkTh;

    CASpyRun *pSR;
};

static void CS_DoWrkSem(CSWrk *pCSWrk)
{
}

static void CS_DoWrkSpy(CSWrk *pCSWrk)
{
}

static DWORD WINAPI CS_WrkTh(void *pArg)
{
    HANDLE hWaitEvts[2];
    CSWrk *pCSWrk = (CSWrk *)pArg;
    DWORD dwWait;
    DWORD dwEvtId;
    DWORD dwEvtsCnt;

    for (;;)
    {
        if (pCSWrk->bStopWrkTh)
        {
            break;
        }

        hWaitEvts[0] = pCSWrk->hWrkSem;
        hWaitEvts[1] = pCSWrk->hSpyRunEvt;
        dwEvtsCnt = sizeof(hWaitEvts) / sizeof(hWaitEvts[0]);
        dwWait = WaitForMultipleObjects(dwEvtsCnt, 
            hWaitEvts, FALSE, INFINITE);
        if (pCSWrk->bStopWrkTh)
        {
            break;
        }

        if (WAIT_FAILED == dwWait || WAIT_TIMEOUT == dwWait)
        {
            Sleep(500);
            continue;
        }

        dwEvtId = dwWait - WAIT_OBJECT_0;
        if (0 == dwEvtId)
        {
            CS_DoWrkSem(pCSWrk);
        }
        else if (1 == dwEvtId)
        {
            CS_DoWrkSpy(pCSWrk);
        }
        else
        {
            Sleep(500);
            continue;
        }
    }

    return CA_THREAD_EXIT_OK; 
}

CAErrno CS_WrkStart(HMODULE hLib, CACfgDatum *pCfgDatum, 
                    CSWrk **pCSWrk)
{
    CRITICAL_SECTION *pWrkCS = NULL;
    CASpyRun *pSR = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    HANDLE hWrkSem = NULL;
    HANDLE hSpyRunEvt = NULL;
    DWORD dwThId;
    CSWrk *pNewCSWrk = NULL;

    *pCSWrk = NULL;
    hWrkSem = CreateSemaphore(NULL, 0, 80000, NULL);
    if (NULL == hWrkSem)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    /* create spy run */
    caErr = CA_SRCreate(&pSR, TRUE);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    CA_SRUpdateState(pSR, CA_SPY_STATE_PREPARE);
        
    hSpyRunEvt = CreateEvent(NULL, FALSE, FALSE, OCASS_EVT_NAME_SPY_RUN);
    if (NULL == hSpyRunEvt)
    {
        CA_SRUpdateState(pSR, CA_SPY_STATE_FAILED);
        funcErr = caErr;
        goto EXIT;
    }

    pNewCSWrk = (CSWrk *)CA_MAlloc(sizeof(CSWrk));
    if (NULL == pNewCSWrk)
    {
        CA_SRUpdateState(pSR, CA_SPY_STATE_FAILED);
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }
    memset(pNewCSWrk, 0, sizeof(CSWrk));

    InitializeCriticalSection(&pNewCSWrk->wrkCS);
    pWrkCS = &pNewCSWrk->wrkCS;
    pNewCSWrk->bStopWrkTh = FALSE;
    pNewCSWrk->hWrkSem = hWrkSem;
    pNewCSWrk->hSpyRunEvt = hSpyRunEvt;
    pNewCSWrk->pSR = pSR;
    pNewCSWrk->hWrkTh = CreateThread(NULL, 0, CS_WrkTh, 
        pNewCSWrk, 0, &dwThId);
    if (NULL == pNewCSWrk->hWrkTh)
    {
        CA_SRUpdateState(pSR, CA_SPY_STATE_FAILED);
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pSR)
        {
            CA_SRClose(pSR);
        }

        if (NULL != hSpyRunEvt)
        {
            CloseHandle(hSpyRunEvt);
        }

        if (NULL != hWrkSem)
        {
            CloseHandle(hWrkSem);
        }

        if (NULL != pWrkCS)
        {
            DeleteCriticalSection(pWrkCS);
        }

        if (NULL != pNewCSWrk)
        {
            CA_MFree(pNewCSWrk);
        }
    }
    else
    {
        *pCSWrk = pNewCSWrk;
    }

    return funcErr;
}

CAErrno CS_WrkStop(CSWrk *pCSWrk)
{  
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_WrkTouchSem(CSWrk *pCSWrk)
{
    BOOL bResult;

    bResult = ReleaseSemaphore(pCSWrk->hWrkSem, 1, NULL);
    return (bResult ? CA_ERR_SUCCESS : CA_ERR_SYS_CALL);
}
