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
    CCWrkState wrkState = CC_WRK_STATE_CORPSE;
    CASpyRun *pSR = NULL;
    CAErrno caErr;

    /* read state from shared mem */
    EnterCriticalSection(&pCWrk->wrkCS);
    if (NULL == pCWrk->pSR)
    {
        caErr = CA_SRAttach(&pSR);
        if (CA_ERR_SUCCESS != caErr)
        {
            wrkState = CC_WRK_STATE_CORPSE;
            goto EXIT;
        }

        wrkState = CC_WRK_STATE_WORKING;
        pCWrk->pSR = pSR;
        goto EXIT;
    }
    wrkState = CC_WRK_STATE_WORKING;

EXIT:
    LeaveCriticalSection(&pCWrk->wrkCS);
    CC_UpdateState(pCWrk, wrkState);
}

static BOOL CH_PInjectCancelCb(void *pCbCtx)
{
    CCWrk *pCWrk = (CCWrk *)pCbCtx;

    if (NULL == pCWrk)
    {
        return FALSE;
    }

    return (pCWrk->bStopWrkTh ? TRUE : FALSE);
}

static void CC_DoInject(CCWrk *pCWrk, DWORD dwProcId)
{
    CAErrno caErr;

    CC_UpdateState(pCWrk, CC_WRK_STATE_INJECTING);
    caErr = CH_PInject(dwProcId, pCWrk, CH_PInjectCancelCb);
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
    case CC_WRK_STATE_NOT_RUNNING:
    case CC_WRK_STATE_NOT_INJECTED:
        return TEXT("Prepare Inject");

    case CC_WRK_STATE_INJECTING:
        return TEXT("Injecting");

    case CC_WRK_STATE_WORKING:
    case CC_WRK_STATE_INJECTED:
        return TEXT("Injected");

    case CC_WRK_STATE_CORPSE:
    case CC_WRK_STATE_INJECT_FAILED:
        return TEXT("Injection failed");

    default:
        return TEXT("Unknown");
    }
}
