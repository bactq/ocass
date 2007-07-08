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

#ifndef _CS_SR_H_
#define _CS_SR_H_ 1

#include "ca_types.h"

typedef int CASpyLogMask;
#define CA_SPY_LOG_NONE         (0)

#define CA_SPY_LOG_ERR          (1<< 0)
#define CA_SPY_LOG_DBG          (1<< 1)
#define CA_SPY_LOG_WARN         (1<< 2)
#define CA_SPY_LOG_INFO         (1<< 3)

#define CA_SPY_LOG_NT_DUMP      (1<<10)
#define CA_SPY_LOG_NT_ADUMP     (1<<11)

#define CA_SPY_LOG_DEL_OLD      (1<<20)
#define CA_SPY_LOG_RENAME_OLD   (1<<21)

typedef enum
{
    CA_SPY_STATE_PREPARE = 0,
    CA_SPY_STATE_RUNNING, 
    CA_SPY_STATE_END,
    CA_SPY_STATE_FAILED,
} CASpyState;

typedef struct _CA_SD
{
    CASpyState  spyState;
    time_t      stateStartTime;
    BOOL        bStateIsDirty;

    BOOL bPauseMon;

    TCHAR szHistoryPath[MAX_PATH];

    TCHAR szSpyLog[MAX_PATH];
    DWORD dwSpyLogTSize;

    TCHAR szSpyNtDump[MAX_PATH];
    DWORD dwSpyNtDumpTSize;

    CASpyLogMask logMask;
} CASpyDatum;

#define CA_SPY_RUN_DATUM_FNAME      (TEXT("ocass_s.datum"))
#define CA_SPY_RUN_LOCK_FNAME       (TEXT("ocass_s.lock"))

typedef struct _CA_SR CASpyRun;

CA_DECLARE(CAErrno) CA_SRCreate(CASpyRun **pSR, BOOL bAutoDupCfg);
CA_DECLARE(CAErrno) CA_SRAttach(CASpyRun **pSR);
CA_DECLARE(CAErrno) CA_SRSmartOpen(CASpyRun **pSR, BOOL bAutoDupCfg);

CA_DECLARE(void)    CA_SRClose(CASpyRun *pSR);

CA_DECLARE(CAErrno)     CA_SRLock(CASpyRun *pSR, BOOL bWr);
CA_DECLARE(CAErrno)     CA_SRUnlock(CASpyRun *pSR);
CA_DECLARE(CASpyDatum*) CA_SRGetDatum(CASpyRun *pSR);
CA_DECLARE(CAErrno)     CA_SRTouch(CASpyRun *pSR);

CA_DECLARE(void)        CA_SRDatumDup(CASpyRun *pSR, CASpyDatum *pDup);
CA_DECLARE(void)        CA_SRUpdateState(CASpyRun *pSR, CASpyState spyState);

#endif /* !defined(_CS_SR_H_) */
