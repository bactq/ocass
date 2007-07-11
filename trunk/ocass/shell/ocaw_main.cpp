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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include "ocaw_main.h"
#include "ocaw_proc.h"

static HINSTANCE g_hInstance = NULL;
static OCAWProc  g_ocawProc = {0};

const HINSTANCE CAS_MGetAppInst(void)
{
    return g_hInstance;
}

OCAWProc* CAS_MGetProcPtr(void)
{
    return &g_ocawProc;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpCmdLine, int nCmdShow)
{
    CAErrno caErr;
    int nProcExit = CA_PROC_EXIT_OK;

    g_hInstance = hInstance;

    InitCommonControls();
    caErr = CAS_PStartup(__argc, __argv, &g_ocawProc);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* XXX panic */
        return CA_PROC_EXIT_INIT_FAILED;
    }

    nProcExit = CAS_PRun(&g_ocawProc);
    CAS_PCleanup(&g_ocawProc);
    return nProcExit;
}
