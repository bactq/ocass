/**
 *
 */

#include <process.h>
#include "ca_types.h"
#include "ca_mm.h"
#include "libocc.h"
#include "cc_inner.h"

CA_DECLARE(CAErrno) CC_Startup(CCWrk **pCWrk)
{
    CRITICAL_SECTION *pWrkCS = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    HANDLE hWrkEvt;
    DWORD dwThId;
    CCWrk *pNewCWrk = NULL;

    *pCWrk = NULL;
    pNewCWrk = (CCWrk *)CA_MAlloc(sizeof(CCWrk));
    if (NULL == pNewCWrk)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    hWrkEvt = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (NULL == hWrkEvt)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    InitializeCriticalSection(&(pNewCWrk->wrkCS));
    pWrkCS = &(pNewCWrk->wrkCS);

    pNewCWrk->bStopWrkTh = FALSE;
    pNewCWrk->hWrkEvt = hWrkEvt;
    pNewCWrk->bStateDirty = TRUE;
    pNewCWrk->stateStartTime = time(NULL);
    pNewCWrk->wrkState = CC_WRK_STATE_IDLE;
    pNewCWrk->hWrkTh = CreateThread(NULL, 0, 
        CC_WrkThread, pNewCWrk, 0, &dwThId);
    if (NULL == pNewCWrk->hWrkTh)
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    *pCWrk = pNewCWrk;
EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pWrkCS)
        {
            DeleteCriticalSection(pWrkCS);
        }
        if (NULL != hWrkEvt)
        {
            CloseHandle(hWrkEvt);
        }
        if (NULL != pNewCWrk)
        {
            CA_MFree(pNewCWrk);
        }
    }
    return funcErr;
}

CA_DECLARE(void) CC_Cleanup(CCWrk *pCWrk)
{
    if (NULL == pCWrk)
    {
        return;
    }

    pCWrk->bStopWrkTh = TRUE;
    CC_Touch(pCWrk);
    WaitForSingleObject(pCWrk->hWrkTh, INFINITE);

    DeleteCriticalSection(&pCWrk->wrkCS);
    CloseHandle(pCWrk->hWrkEvt);
    CloseHandle(pCWrk->hWrkTh);
    CA_MFree(pCWrk);
}

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, BOOL bClearDirtyFlag,
                             CCWrkStateDesc *pStateDesc)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    pStateDesc->startTime = pCWrk->stateStartTime;
    pStateDesc->wrkState = pCWrk->wrkState;
    pStateDesc->bIsDirty = pCWrk->bStateDirty;
    if (bClearDirtyFlag)
    {
        pCWrk->bStateDirty = FALSE;
    }
    LeaveCriticalSection(&pCWrk->wrkCS);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CC_Touch(CCWrk *pCWrk)
{
    BOOL bResult;
    bResult = SetEvent(pCWrk->hWrkEvt);
    return (bResult ? CA_ERR_SUCCESS :CA_ERR_SYS_CALL);
}

CA_DECLARE(CAErrno) CC_StateClear(CCWrk *pCWrk)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    pCWrk->bStateDirty = FALSE;
    LeaveCriticalSection(&pCWrk->wrkCS);
    return CA_ERR_SUCCESS;
}
