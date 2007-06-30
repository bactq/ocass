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

static void CC_DoWrk(CCWrk *pCWrk, CCWrkMod wrkMod)
{
    CAErrno caErr;
    DWORD dwOFCProcId;
    BOOL bResult;

    if (!(CC_WRK_MOD_DETECT & wrkMod))
    {
        return;
    }

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
        if (CC_WRK_MOD_INJECT & wrkMod)
        {
            CC_DoInject(pCWrk, dwOFCProcId);
        }
        else 
        {
            CC_UpdateState(pCWrk, CC_WRK_STATE_NOT_INJECTED);
        }
        return;
    }

    CC_UpdateSpyWrkState(pCWrk, dwOFCProcId);
}

DWORD WINAPI CC_WrkThread(void *pThArg)
{
    CCWrkMod wrkMod;
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

        CC_GetWrkMod(pCWrk, &wrkMod);
        if (CC_WRK_MOD_PAUSE & wrkMod)
        {
            continue;
        }

        CC_DoWrk(pCWrk, wrkMod);
    }

    return dwThExit;
}

CA_DECLARE(const TCHAR*) CC_StateDesc(CCWrkState wrkState)
{
    switch (wrkState)
    {
    case CC_WRK_STATE_IDLE:
        return TEXT("IDLE");
    case CC_WRK_STATE_NOT_RUNNING:
        return TEXT("Not running");
    case CC_WRK_STATE_NOT_INJECTED:
        return TEXT("Not injected");
    case CC_WRK_STATE_INJECTING:
        return TEXT("Injecting");
    case CC_WRK_STATE_INJECTED:
        return TEXT("Injected");
    case CC_WRK_STATE_INJECT_FAILED:
        return TEXT("Inject failed");
    case CC_WRK_STATE_CORPSE:
        return TEXT("Corpse");
    case CC_WRK_STATE_WORKING:
        return TEXT("Working");
    default:
        return TEXT("Unknown");
    }
}
