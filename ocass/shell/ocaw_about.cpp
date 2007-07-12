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

#include "ca_ver.h"
#include "ocaw_main.h"
#include "ocaw_about.h"
#include "resource.h"

const TCHAR *OCAS_ABOUT_TXT = 
        TEXT("test\r\n"
             "test2");

static BOOL OCAS_AboutOnClose(HWND hWnd)
{
    SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    return TRUE;
}

static BOOL OCAS_AboutOnInit(HWND hWnd)
{
    OCAS_SetDlgItemTxt(hWnd, IDC_STATIC_VER, CA_VER_PRODUCT_STR);
    OCAS_SetDlgItemTxt(hWnd, IDC_STATIC_BTM, __DATE__);
    OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_ABOUT_TXT, OCAS_ABOUT_TXT);
    return TRUE;
}

static BOOL CALLBACK OCAS_AboutDlgProc(HWND hWnd, UINT nMsg, 
                                       UINT wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        return OCAS_AboutOnInit(hWnd);

    case WM_CLOSE:
        EndDialog(hWnd, TRUE);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUT_ABOUT_CLOSE:
            return OCAS_AboutOnClose(hWnd);

        default:
            return FALSE;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

BOOL OCAS_OnMainDlgAbout(HWND hWnd)
{
    INT_PTR nDlgResult;

    nDlgResult = DialogBox(CAS_MGetAppInst(), MAKEINTRESOURCE(IDD_DLG_ABOUT), 
        hWnd, (DLGPROC)OCAS_AboutDlgProc);
    if (0 != nDlgResult)
    {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    return TRUE;
}
