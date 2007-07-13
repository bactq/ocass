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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include "ocaw_main.h"
#include "ocaw_proc.h"
#include "ocaw_panic.h"

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
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Startup failed. Can't load run time library or config."
                 "Last Error code (%u). "), caErr);
        return CA_PROC_EXIT_INIT_FAILED;
    }

    nProcExit = CAS_PRun(&g_ocawProc);
    CAS_PCleanup(&g_ocawProc);
    return nProcExit;
}
