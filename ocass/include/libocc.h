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

#ifndef _LIBOCC_H_
#define _LIBOCC_H_ 1

#include "liboca.h"
#include "liboch.h"
#include "ca_cfg.h"

typedef struct _CC_Wrk CCWrk;

typedef enum
{
    CC_WRK_STATE_IDLE = 0,
    CC_WRK_STATE_NOT_RUNNING, 
    CC_WRK_STATE_NOT_INJECTED, 
    CC_WRK_STATE_INJECTING, 
    CC_WRK_STATE_INJECTED, 
    CC_WRK_STATE_INJECT_FAILED, 
    CC_WRK_STATE_CORPSE,
    CC_WRK_STATE_WORKING,
} CCWrkState;

typedef int CCWrkMod;
#define CC_WRK_MOD_DETECT       (1<< 0)
#define CC_WRK_MOD_INJECT       (1<< 1)
#define CC_WRK_MOD_PAUSE        (1<< 2)

#define CC_WRK_MOD_NORMAL       (CC_WRK_MOD_DETECT|CC_WRK_MOD_INJECT)
#define CC_WRK_MOD_MON          (CC_WRK_MOD_DETECT)
#define CC_WRK_MOD_SAFE         (CC_WRK_MOD_PAUSE)

typedef struct _CC_WRK_STATE_DESC
{
    CCWrkState wrkState;
    time_t startTime;

    CCWrkMod wrkMod;
    BOOL bIsDirty;
} CCWrkStateDesc;

CA_DECLARE(CAErrno) CC_Startup(CCWrkMod wrkMod, CCWrk **pCWrk);
CA_DECLARE(void)    CC_Cleanup(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, BOOL bClearDirtyFlag,
                             CCWrkStateDesc *pStateDesc);

CA_DECLARE(CAErrno) CC_SetWrkMod(CCWrk *pCWrk, CCWrkMod wrkMod);
CA_DECLARE(CAErrno) CC_GetWrkMod(CCWrk *pCWrk, CCWrkMod *pWrkMod);
CA_DECLARE(CAErrno) CC_SetPauseFlag(CCWrk *pCWrk, BOOL bPause);
CA_DECLARE(BOOL) CC_IsPause(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_StateClear(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_Touch(CCWrk *pCWrk);

CA_DECLARE(const TCHAR*) CC_StateDesc(CCWrkState wrkState);

CA_DECLARE(CAErrno) CC_UpdateCfg(CCWrk *pCWrk, CACfgDatum *pCfgDatum);

#endif /* !defined(_LIBOCC_H_) */
