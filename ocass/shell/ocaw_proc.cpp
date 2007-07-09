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

#include "ocaw_main.h"
#include "ocaw_proc.h"
#include "ocaw_wrk.h"

int OCAS_PUseage(OCAWProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_USEAGE;
    /* XXX show useage */
    return nProcExit;
}

int OCAS_PWakeUp(OCAWProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;
    /* XXX wake up */
    return nProcExit;
}

CAErrno CAS_PStartup(int nArgc, char **pArgv, 
                     OCAWProc *pProc)
{
    memset(pProc, 0, sizeof(OCAWProc));
    pProc->argc = nArgc;
    pProc->argv = pArgv;
    pProc->dwShellProcId = GetCurrentProcessId();
    pProc->shellProcType = OCASP_TYPE_WRK;

    return CA_ERR_SUCCESS;
}

void CAS_PCleanup(OCAWProc *pProc)
{
}

int CAS_PRun(OCAWProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    switch (pProc->shellProcType)
    {
    case OCASP_TYPE_WRK:
        return OCAS_PWrk(pProc);

    case OCASP_TYPE_WAKEUP:
        return OCAS_PWakeUp(pProc);

    default:
    case OCASP_TYPE_USEAGE:
        return OCAS_PUseage(pProc);
    }
}
