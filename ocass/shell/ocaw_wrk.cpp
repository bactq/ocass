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
#include "resource.h"

BOOL OCAS_OnMainDlgCmd(HWND hWnd, UINT wParam, LPARAM lParam)
{
    DWORD dwCmd = LOWORD(wParam);
    switch (dwCmd)
    {
    case ID_SYSTEM_EXIT:
        SendMessage(hWnd, WM_CLOSE, NULL, NULL);
        break;
    }
    return TRUE;
}

static BOOL CALLBACK OCAS_MainDlgProc(HWND hWnd, UINT nMsg, 
                                      WPARAM wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    case WM_COMMAND:
        return OCAS_OnMainDlgCmd(hWnd, wParam, lParam);

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;

    default:
        return FALSE;
    }
}

int OCAS_PWrk(OCAWProc *pProc)
{
    HWND hMainDlg = NULL;
    BOOL bResult;
    MSG  wndMsg;
    int nProcExit = CA_PROC_EXIT_OK;

    hMainDlg = CreateDialog(CAS_MGetAppInst(), 
        MAKEINTRESOURCE(IDD_DLG_MAIN), 0, OCAS_MainDlgProc);
    if (NULL == hMainDlg)
    {
        /* XXX  */
        return CA_PROC_EXIT_INIT_FAILED;
    }

    ShowWindow(hMainDlg, SW_SHOWNORMAL);   
    for (;;)
    {
        bResult = GetMessage(&wndMsg, 0, 0, 0);
        if (!bResult)
        {
            break;
        }

        bResult = IsDialogMessage(hMainDlg, &wndMsg);
        if (!bResult)
        {
            TranslateMessage(&wndMsg);
            DispatchMessage(&wndMsg);
        }
    }

    return nProcExit;
}