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
#include "ocaw_opt.h"
#include "ocaw_misc.h"
#include "resource.h"

static BOOL OCAS_CfgOnBtnOkClick(HWND hWnd)
{
    /* save config */

    SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    return TRUE;
}

static BOOL OCAS_CfgOnInit(HWND hWnd)
{
    /* XXX */
    return TRUE;
}

static BOOL OCAS_CfgOnCPNameClick(HWND hWnd)
{
    TCHAR szFName[MAX_PATH];
    BOOL bResult;

    bResult = OCAS_SelFile(hWnd, TEXT("Programs (*.exe)\0*.exe\0"), 
        szFName, sizeof(szFName) / sizeof(szFName[0]));
    if (!bResult)
    {
        return FALSE;
    }

    /* XXX sel file */
    return TRUE;
}

static BOOL CALLBACK OCAS_CfgProc(HWND hWnd, UINT nMsg, 
                                  UINT wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        return OCAS_CfgOnInit(hWnd);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BTN_CFG_OK:
            return OCAS_CfgOnBtnOkClick(hWnd);

        case IDC_BTN_CFG_CANCEL:
            EndDialog(hWnd, TRUE);
            return TRUE;

        case IDC_BTN_BRCPN:
            return OCAS_CfgOnCPNameClick(hWnd);

        default:
            return FALSE;
        }
        return TRUE;

    case WM_CLOSE:
        EndDialog(hWnd, TRUE);
        return TRUE;

    default:
        return FALSE;
    }
}

BOOL OCAS_OnMainDlgCfg(HWND hWnd)
{
    INT_PTR nDlgResult;

    nDlgResult = DialogBox(CAS_MGetAppInst(), MAKEINTRESOURCE(IDD_DLG_CFG), 
        hWnd, (DLGPROC)OCAS_CfgProc);
    if (0 != nDlgResult)
    {
        InvalidateRect(hWnd, NULL, TRUE);
    }
    return TRUE;
}
