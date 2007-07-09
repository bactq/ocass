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

#ifndef _OCAW_RPOC_H_
#define _OCAW_RPOC_H_ 1

#include "ca_types.h"

typedef enum
{
    OCASP_TYPE_WRK = 0, 
    OCASP_TYPE_WAKEUP, 
    OCASP_TYPE_USEAGE, 
} OCASPType;

typedef struct _OCAWProcSubWrk
{
    HWND hMWnd;
} OCAWProcSubWrk;

typedef struct _OCAWProcSubWakeUp
{
    int x;
} OCAWProcSubWakeUp;

typedef struct _OCAWProc
{
    DWORD dwShellProcId;
    int argc;
    char **argv;
    OCASPType shellProcType;
    union
    {
        OCAWProcSubWrk wrk;
        OCAWProcSubWakeUp wakeup;
    } SubData;
} OCAWProc;

CAErrno CAS_PStartup(int nArgc, char **pArgv, 
                     OCAWProc *pProc);

void CAS_PCleanup(OCAWProc *pProc);

int CAS_PRun(OCAWProc *pProc);

#endif /* !defined(_OCAW_RPOC_H_) */
