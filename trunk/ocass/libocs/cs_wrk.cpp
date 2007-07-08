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

#include "cs_wrk.h"
#include "ca_evts.h"
#include "ca_mm.h"
#include "ca_sr.h"
#include "cs_log.h"

struct _CSWrk
{
    CRITICAL_SECTION wrkCS;
    HANDLE  hSpyRunEvt;
    HANDLE  hWrkTh;
    HANDLE  hWrkSem;
    BOOL    bStopWrkTh;

    CSProtoCache protoCache;

    CASpyDatum spyDatum;
    CASpyRun *pSR;
};

static void CS_DoWrkSem(CSWrk *pCSWrk)
{
    BOOL bIsPauseMon;

    EnterCriticalSection(&pCSWrk->wrkCS);
    bIsPauseMon = pCSWrk->spyDatum.bPauseMon;
    CS_ProtoCacheProcess(&pCSWrk->protoCache, bIsPauseMon);
    LeaveCriticalSection(&pCSWrk->wrkCS);
}

static void CS_DoWrkSpy(CSWrk *pCSWrk)
{
    CSLogCfg csLogCfg;
    BOOL bStateIsDirty;

    /* update config */
    EnterCriticalSection(&pCSWrk->wrkCS);
    bStateIsDirty = pCSWrk->spyDatum.bStateIsDirty;
    CA_SRDatumDup(pCSWrk->pSR, &pCSWrk->spyDatum);
    if (pCSWrk->spyDatum.bStateIsDirty)
    {        
        /* update log config */
        csLogCfg.pszSpyLogFName = pCSWrk->spyDatum.szSpyLog;
        csLogCfg.dwSpyLogTSize = pCSWrk->spyDatum.dwSpyLogTSize;
        csLogCfg.pszNtDumpLogFName = pCSWrk->spyDatum.szSpyNtDump;
        csLogCfg.dwNtDumpLogTSize = pCSWrk->spyDatum.dwSpyNtDumpTSize;
        csLogCfg.spyLogMask = pCSWrk->spyDatum.logMask;
    }
    LeaveCriticalSection(&pCSWrk->wrkCS);

    if (bStateIsDirty)
    {
        CS_LogUpdateCfg(&csLogCfg);
    }
}

static DWORD WINAPI CS_WrkTh(void *pArg)
{
    HANDLE hWaitEvts[2];
    CSWrk *pCSWrk = (CSWrk *)pArg;
    DWORD dwWait;
    DWORD dwEvtId;
    DWORD dwEvtsCnt;

    CA_SRUpdateState(pCSWrk->pSR, CA_SPY_STATE_RUNNING);
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

    CA_SRUpdateState(pCSWrk->pSR, CA_SPY_STATE_END);
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
    CA_SRDatumDup(pSR, &(pNewCSWrk->spyDatum));
    caErr = CS_ProtoCacheStartup(&pNewCSWrk->protoCache);
    if (CA_ERR_SUCCESS != funcErr)
    {
        CA_SRUpdateState(pSR, CA_SPY_STATE_FAILED);
        funcErr = caErr;
        goto EXIT;
    }

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
    pCSWrk->bStopWrkTh = TRUE;
    CS_WrkTouchSem(pCSWrk, 1);
    WaitForSingleObject(pCSWrk->hWrkTh, INFINITE);
    CS_ProtoCacheCleanup(&pCSWrk->protoCache);
    
    CloseHandle(pCSWrk->hWrkSem);
    CloseHandle(pCSWrk->hSpyRunEvt);
    CloseHandle(pCSWrk->hWrkTh);
    DeleteCriticalSection(&pCSWrk->wrkCS);
    CA_SRClose(pCSWrk->pSR);
    CA_MFree(pCSWrk);
    return CA_ERR_SUCCESS;
}

CAErrno CS_WrkTouchSem(CSWrk *pCSWrk, DWORD dwTouchCnt)
{
    BOOL bResult;

    bResult = ReleaseSemaphore(pCSWrk->hWrkSem, dwTouchCnt, NULL);
    return (bResult ? CA_ERR_SUCCESS : CA_ERR_SYS_CALL);
}

const CASpyDatum* CS_WrkGetSD(const CSWrk *pCSWrk)
{
    if (NULL == pCSWrk)
    {
        return NULL;
    }

    return &(pCSWrk->spyDatum);
}

void CS_WrkAddRawBuf(CSWrk *pCSWrk, CSProtoBuf *pProtoBuf, 
                     CSProtoType protoType)
{
    EnterCriticalSection(&pCSWrk->wrkCS);
    CS_ProtoCacheRawAdd(&pCSWrk->protoCache, pProtoBuf, protoType);
    LeaveCriticalSection(&pCSWrk->wrkCS);
}

BOOL CS_WrkIsPause(CSWrk *pCSWrk)
{
    BOOL bIsPauseMon;

    if (NULL == pCSWrk)
    {
        return FALSE;
    }

    EnterCriticalSection(&pCSWrk->wrkCS);
    bIsPauseMon = pCSWrk->spyDatum.bPauseMon;
    LeaveCriticalSection(&pCSWrk->wrkCS);
    return bIsPauseMon;
}
