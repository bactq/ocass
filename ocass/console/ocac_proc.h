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

#ifndef _OCAC_PROC_H_
#define _OCAC_PROC_H_ 1

#include "liboca.h"

typedef enum
{    
    OCAC_PROC_TYPE_WORKER = 0,
    OCAC_PROC_TYPE_KILLER,
    OCAC_PROC_TYPE_USAGE,
} OCACProcType;

typedef struct _OCAC_PROC_
{
    OCACProcType procType;
    TCHAR szProcWrkPath[MAX_PATH];
    TCHAR szProcCfgFName[MAX_PATH];
} OCACProc;

CAErrno CAC_PStartup(int nArgc, char **pArgv, OCACProc *pProc);
CAErrno CAC_PCleanup(OCACProc *pProc);

#endif /* !defined(_OCAC_PROC_H_) */
