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
#include "ocaw_opt.h"
#include "ocaw_about.h"
#include "ocaw_evts.h"
#include "resource.h"

BOOL OCAS_OnMainDlgInit(HWND hWnd)
{
    HICON hAppIcon; 
    
    hAppIcon = LoadIcon(CAS_MGetAppInst(), MAKEINTRESOURCE(IDI_ICON_MAIN));
    if (NULL != hAppIcon)
    {
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hAppIcon);
    }
    
    return TRUE;
}

BOOL OCAS_OnMainDlgCmdEvt(HWND hWnd, UINT wParam, LPARAM lParam, 
                          DWORD dwCtlId)
{
    switch (dwCtlId)
    {
    case ID_SYSTEM_EXIT:
        DestroyWindow(hWnd);
        return TRUE;

    case ID_SYSTEM_OPTIONS:
        return OCAS_OnMainDlgCfg(hWnd);

    case ID_ABOUT:
        return OCAS_OnMainDlgAbout(hWnd);

    case ID_POPMENU_OPEN:
        return OCAS_MainDlgShow(hWnd);

    case ID_POPMENU_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;

    default:
        return FALSE;
    }
}

BOOL OCAS_OnMainDlgClose(HWND hWnd)
{
    return OCAS_MainDlgHide(hWnd);
}

BOOL OCAS_MainDlgHide(HWND hWnd)
{
    ShowWindow(hWnd, SW_HIDE);
    return TRUE;
}

BOOL OCAS_MainDlgShow(HWND hWnd)
{
    ShowWindow(hWnd, SW_SHOWNORMAL);
    SetForegroundWindow(hWnd);
    return TRUE;
}

BOOL OCAS_ShowTrayPopMenu(HWND hWnd)
{
    HMENU hMenu = CreatePopupMenu();
    POINT cursorPos;
    BOOL bResult;

    bResult = GetCursorPos(&cursorPos);
    if (!bResult)
    {
        return FALSE;
    }

    if (NULL == hMenu)
    {
        /* XXX write log */
        return FALSE;
    }

    OCAS_AppendMenuItems(hMenu, g_popMenuItems, 
        sizeof(g_popMenuItems) / sizeof(g_popMenuItems[0]));

    bResult = TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, 
        cursorPos.x, cursorPos.y, 0, hWnd, NULL);
    if (!bResult)
    {
        /* XXX write log */
    }

    DestroyMenu(hMenu);
    return TRUE;
}