/**
 *
 */

#include "ca_types.h"
#include "ca_ofc.h"
#include "liboch.h"
#include "cc_inner.h"

static void CC_UpdateState(CCWrk *pCWrk, CCWrkState wrkState)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    if (wrkState != pCWrk->wrkState)
    {
        pCWrk->stateStartTime = time(NULL);
        pCWrk->wrkState = wrkState;
        pCWrk->bStateDirty = TRUE;
    }
    LeaveCriticalSection(&pCWrk->wrkCS);
}

static void CC_UpdateSpyWrkState(CCWrk *pCWrk, DWORD dwProcId)
{
    /* read state from shared mem */
    CC_UpdateState(pCWrk, CC_WRK_STATE_WORKING);
}

static void CC_DoInject(CCWrk *pCWrk, DWORD dwProcId)
{
    CAErrno caErr;

    CC_UpdateState(pCWrk, CC_WRK_STATE_INJECTING);
    caErr = CH_PInject(dwProcId, 0);
    if (CA_ERR_SUCCESS == caErr)
    {
        CC_UpdateState(pCWrk, CC_WRK_STATE_INJECTED);
    }
    else
    {
        CC_UpdateState(pCWrk, CC_WRK_STATE_INJECT_FAILED);
    }
    
}

static void CC_DoWrk(CCWrk *pCWrk)
{
    CAErrno caErr;
    DWORD dwOFCProcId;
    BOOL bResult;

    caErr = CA_OFCGetRunProcId(&dwOFCProcId);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* not running */
        CC_UpdateState(pCWrk, CC_WRK_STATE_NOT_RUNNING);
        return;
    }

    bResult = CA_OFCProcIsAttached(dwOFCProcId);
    if (!bResult)
    {
        CC_DoInject(pCWrk, dwOFCProcId);
        return;
    }

    CC_UpdateSpyWrkState(pCWrk, dwOFCProcId);
}

DWORD WINAPI CC_WrkThread(void *pThArg)
{
    CCWrk *pCWrk = (CCWrk *)pThArg;
    DWORD dwThExit = CA_THREAD_EXIT_OK;
    DWORD dwWait;
    DWORD dwWrkCycle = 1000 * 3;

    for (;;)
    {
        if (pCWrk->bStopWrkTh)
        {
            break;
        }

        dwWait = WaitForSingleObject(pCWrk->hWrkEvt, dwWrkCycle);
        if (pCWrk->bStopWrkTh)
        {
            break;
        }

        if (WAIT_FAILED == dwWait)
        {
            Sleep(1000);
        }

        CC_DoWrk(pCWrk);
    }

    return dwThExit;
}
