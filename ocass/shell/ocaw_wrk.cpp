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
#include "ocaw_evts.h"
#include "ocaw_panic.h"
#include "resource.h"

BOOL OCAS_OnMainDlgCmd(HWND hWnd, UINT wParam, LPARAM lParam)
{
    DWORD dwCmd = LOWORD(wParam);
    return OCAS_OnMainDlgCmdEvt(hWnd, wParam, lParam, dwCmd);
}

BOOL OCAS_OnMainDlgNotifyIconMsg(HWND hWnd, UINT wParam, LPARAM lParam)
{
    switch (lParam)
    {
    case WM_RBUTTONDOWN:
        return OCAS_ShowTrayPopMenu(hWnd);
    case WM_LBUTTONDBLCLK:
        return OCAS_MainDlgShow(hWnd);
    default:
        return TRUE;
    }
}

static BOOL CALLBACK OCAS_MainDlgProc(HWND hWnd, UINT nMsg, 
                                      WPARAM wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        return OCAS_OnMainDlgInit(hWnd);

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;

    case WM_COMMAND:
        return OCAS_OnMainDlgCmd(hWnd, wParam, lParam);

    case OCAW_MSG_NOTIFY_ICON:
        return OCAS_OnMainDlgNotifyIconMsg(hWnd, wParam, lParam);

    case WM_CLOSE:
        return OCAS_OnMainDlgClose(hWnd);        

    default:
        return FALSE;
    }
}

int OCAS_PWrk(OCAWProc *pProc)
{
    NOTIFYICONDATA notifyIconData;
    HWND hMainDlg = NULL;
    BOOL bResult;
    MSG  wndMsg;
    int nProcExit = CA_PROC_EXIT_OK;

    hMainDlg = CreateDialog(CAS_MGetAppInst(), 
        MAKEINTRESOURCE(IDD_DLG_MAIN), NULL, OCAS_MainDlgProc);
    if (NULL == hMainDlg)
    {
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Create Main dialog failed. System Last Error is %u"), 
            GetLastError());
        return CA_PROC_EXIT_INIT_FAILED;
    }

    pProc->hMainDlg = hMainDlg;
    if (!pProc->bIsBackground)
    {
        ShowWindow(hMainDlg, SW_SHOWNORMAL);
    }

    /* add notify icon */
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = LoadIcon(CAS_MGetAppInst(), 
        MAKEINTRESOURCE(IDI_ICON_MAIN));
    notifyIconData.hWnd = hMainDlg;
    notifyIconData.szTip[0] = '\0';
    notifyIconData.uID = OCAW_MAIN_NOTIFY_ICON_ID;
    notifyIconData.uCallbackMessage = OCAW_MSG_NOTIFY_ICON;
    notifyIconData.uFlags = NIF_ICON|NIF_MESSAGE;
    bResult = Shell_NotifyIcon(NIM_ADD, &notifyIconData);
    if (!bResult)
    {
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Create Main notify icon failed. System Last Error is %u"), 
            GetLastError());
        return CA_PROC_EXIT_INIT_FAILED;
    }

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

    /* remove notify icon */
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.uID = OCAW_MAIN_NOTIFY_ICON_ID;
    notifyIconData.uFlags = NIF_ICON;
    Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
    return nProcExit;
}
