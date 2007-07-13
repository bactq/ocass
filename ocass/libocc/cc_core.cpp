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

#include <process.h>

#include "liboca.h"
#include "ca_types.h"
#include "ca_mm.h"
#include "ca_str.h"
#include "libocc.h"
#include "cc_inner.h"

static CAErrno CC_UpdateCfgToShMM(CCWrk *pCWrk, CACfgDatum *pCfgDatum)
{
    CASpyDatum *pSDatum;
    CASpyRun *pSR = NULL;
    CAErrno caErr;
    CAErrno funcErr = CA_ERR_SUCCESS;
    BOOL bNeedTouch = FALSE;

    if (NULL == pCWrk || NULL == pCWrk->pSR)
    {
        return CA_ERR_BAD_SEQ;
    }

    pSR = pCWrk->pSR;
    caErr = CA_SRLock(pSR, TRUE);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN|CA_RTLOG_OSERR, 
            TEXT("core update pause state failed. can't open the flock"));
        return caErr;
    }

    pSDatum = CA_SRGetDatum(pSR);
    if (NULL == pSDatum)
    {
        funcErr = CA_ERR_BAD_ARG;
        goto EXIT;
    }

    pSDatum->bStateIsDirty = TRUE;
    CA_TruncateStrnCpy(pSDatum->szHistoryPath, pCfgDatum->szHistoryPath, 
        sizeof(pSDatum->szHistoryPath) / sizeof(pSDatum->szHistoryPath[0]));
    CA_TruncateStrnCpy(pSDatum->szSpyLog, pCfgDatum->szSpyLog,
        sizeof(pSDatum->szSpyLog) / sizeof(pSDatum->szSpyLog[0]));
    CA_TruncateStrnCpy(pSDatum->szSpyNtDump, pCfgDatum->szSpyNtDump,
        sizeof(pSDatum->szSpyNtDump) / sizeof(pSDatum->szSpyNtDump[0]));

    pSDatum->dwSpyLogTSize = pCfgDatum->dwSpyLogTSize;
    pSDatum->dwSpyNtDumpTSize = pCfgDatum->dwSpyNtDumpTSize;
    pSDatum->logMask = pCfgDatum->spyLogMask;
    bNeedTouch = TRUE;
EXIT:
    CA_SRUnlock(pSR);
    if (bNeedTouch)
    {
        CA_SRTouch(pSR);
    }
    return funcErr;
}

CA_DECLARE(CAErrno) CC_Startup(CCWrkMod wrkMod, CCWrk **pCWrk)
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
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR|CA_RTLOG_OSERR, 
            TEXT("core startup failed. no memory"));

        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }
    memset(pNewCWrk, 0, sizeof(CCWrk));

    hWrkEvt = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (NULL == hWrkEvt)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR|CA_RTLOG_OSERR, 
            TEXT("core startup failed. create event failed"));

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
    pNewCWrk->wrkMod = wrkMod;
    pNewCWrk->pSR = NULL;
    pNewCWrk->hWrkTh = CreateThread(NULL, 0, 
        CC_WrkThread, pNewCWrk, 0, &dwThId);
    if (NULL == pNewCWrk->hWrkTh)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR|CA_RTLOG_OSERR, 
            TEXT("core startup failed. create thread failed"));

        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
        TEXT("core startup success, core thread id %u"), dwThId);

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
    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
        TEXT("core start cleanup"));

    if (NULL == pCWrk)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN, 
            TEXT("core cleanup failed. bad arg"));
        return;
    }

    pCWrk->bStopWrkTh = TRUE;
    CC_Touch(pCWrk);
    WaitForSingleObject(pCWrk->hWrkTh, INFINITE);

    DeleteCriticalSection(&pCWrk->wrkCS);
    CloseHandle(pCWrk->hWrkEvt);
    CloseHandle(pCWrk->hWrkTh);
    if (NULL != pCWrk->pSR)
    {
        CA_SRClose(pCWrk->pSR);
    }
    CA_MFree(pCWrk);

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
            TEXT("core cleanup successed"));
}

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, BOOL bClearDirtyFlag,
                             CCWrkStateDesc *pStateDesc)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    pStateDesc->startTime = pCWrk->stateStartTime;
    pStateDesc->wrkState = pCWrk->wrkState;
    pStateDesc->bIsDirty = pCWrk->bStateDirty;
    pStateDesc->wrkMod = pCWrk->wrkMod;

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
    if (!bResult)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN|CA_RTLOG_OSERR, 
            TEXT("work core touch wrk evt failed."));
    }
    return (bResult ? CA_ERR_SUCCESS :CA_ERR_SYS_CALL);
}

CA_DECLARE(CAErrno) CC_StateClear(CCWrk *pCWrk)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    pCWrk->bStateDirty = FALSE;
    LeaveCriticalSection(&pCWrk->wrkCS);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CC_SetWrkMod(CCWrk *pCWrk, CCWrkMod wrkMod)
{
    EnterCriticalSection(&pCWrk->wrkCS);
    pCWrk->wrkMod = wrkMod;
    LeaveCriticalSection(&pCWrk->wrkCS);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CC_GetWrkMod(CCWrk *pCWrk, CCWrkMod *pWrkMod)
{
    if (NULL == pWrkMod)
    {
        return CA_ERR_BAD_ARG;
    }

    EnterCriticalSection(&pCWrk->wrkCS);
    *pWrkMod = pCWrk->wrkMod;
    LeaveCriticalSection(&pCWrk->wrkCS);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CC_SetPauseFlag(CCWrk *pCWrk, BOOL bPause)
{
    CASpyDatum *pSHDatum;
    CASpyRun *pSR;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    BOOL bNeedTouch = FALSE;

    EnterCriticalSection(&pCWrk->wrkCS);
    pCWrk->wrkMod = (bPause ? CC_WRK_MOD_SAFE : CC_WRK_MOD_NORMAL);
    if (NULL == pCWrk->pSR)
    {
        goto EXIT;
    }

    pSR = pCWrk->pSR;
    caErr = CA_SRLock(pSR, TRUE);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN|CA_RTLOG_OSERR, 
            TEXT("core update pause state failed. can't open the flock"));

        funcErr = caErr;
        goto EXIT;
    }

    pSHDatum = CA_SRGetDatum(pCWrk->pSR);
    if (NULL != pSHDatum)
    {
        pSHDatum->bPauseMon = bPause;
        bNeedTouch = TRUE;
    }
    CA_SRUnlock(pSR);

    if (bNeedTouch)
    {
        CA_SRTouch(pSR);
    }
EXIT:
    LeaveCriticalSection(&pCWrk->wrkCS);
    return funcErr;
}

CA_DECLARE(BOOL) CC_IsPause(CCWrk *pCWrk)
{
    CCWrkMod wrkMod;

    EnterCriticalSection(&pCWrk->wrkCS);
    wrkMod = pCWrk->wrkMod;
    LeaveCriticalSection(&pCWrk->wrkCS);
    return (wrkMod & CC_WRK_MOD_PAUSE ? TRUE : FALSE);
}

CA_DECLARE(CAErrno) CC_UpdateCfg(CCWrk *pCWrk, const TCHAR *pszCfgFName, 
                                 CACfgDatum *pCfgDatum)
{
    CACfgDatum oldCfgDatum;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;

    caErr = CA_CfgReCalculate(pCfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("ReCalculate config data failed, error code %u"), 
            caErr);

        return caErr;
    }

    CA_CfgDupRT(&oldCfgDatum);

    /* update config to file */
    caErr = CA_CfgShWr(pszCfgFName, &oldCfgDatum, pCfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("write config config data to (%s) failed, error code %u"), 
            pszCfgFName, caErr);
    }

    /* update config to rt data */
    CA_CfgSetRT(pCfgDatum);

    /* update config to sh data */
    EnterCriticalSection(&pCWrk->wrkCS);    
    funcErr = CC_UpdateCfgToShMM(pCWrk, pCfgDatum);
    LeaveCriticalSection(&pCWrk->wrkCS);
    return funcErr;
}
