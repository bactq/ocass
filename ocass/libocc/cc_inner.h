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

#ifndef _CC_INNER_H_
#define _CC_INNER_H_ 1

#include "libocc.h"
#include "ca_sr.h"

struct _CC_Wrk
{
    HANDLE              hWrkTh;
    CRITICAL_SECTION    wrkCS;
    HANDLE              hWrkEvt;
    BOOL                bStopWrkTh;

    CCWrkMod    wrkMod;
    BOOL        bStateDirty;
    CCWrkState  wrkState;
    time_t      stateStartTime;

    CASpyRun    *pSR;
};


DWORD WINAPI CC_WrkThread(void *pThArg);

#endif /* !defined(_CC_INNER_H_) */
