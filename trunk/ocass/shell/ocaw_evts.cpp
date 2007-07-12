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

#include "ca_cfg.h"
#include "ca_str.h"
#include "libocc.h"
#include "ocaw_main.h"
#include "ocaw_proc.h"
#include "ocaw_wrk.h"
#include "ocaw_opt.h"
#include "ocaw_about.h"
#include "ocaw_evts.h"
#include "ocaw_misc.h"
#include "resource.h"

BOOL OCAS_OnUpdateMonBtnState(HWND hWnd)
{
    OCAWProc *pProc = CAS_MGetProcPtr();
    BOOL bIsPause;

    if (NULL == pProc || NULL == pProc->pCCWrk)
    {
        bIsPause = FALSE;
    }
    else
    {
        bIsPause = CC_IsPause(pProc->pCCWrk);
    }


    if (bIsPause)
    {
        OCAS_SetDlgItemEnable(hWnd, IDC_BTN_MONCTLP, FALSE);
        OCAS_SetDlgItemEnable(hWnd, IDC_BTN_MONCTLS, TRUE);
    }
    else
    {
        OCAS_SetDlgItemEnable(hWnd, IDC_BTN_MONCTLP, TRUE);
        OCAS_SetDlgItemEnable(hWnd, IDC_BTN_MONCTLS, FALSE);
    }

    return TRUE;
}

BOOL OCAS_ChangeHistoryPath(HWND hWnd, TCHAR *pszHistoryPath)
{
    CACfgDatum cfgDatum;
    OCAWProc *pProc = CAS_MGetProcPtr();
    CAErrno caErr;
    int nResult;

    if (NULL == pszHistoryPath || '\0' == pszHistoryPath[0] ||
        NULL == pProc || NULL == pProc->pCCWrk)
    {
        return FALSE;
    }

    CA_CfgDupRT(&cfgDatum);
    nResult = CA_SNPrintf(cfgDatum.szHistoryPath, 
        sizeof(cfgDatum.szHistoryPath) / sizeof(cfgDatum.szHistoryPath[0]), 
        TEXT("%s"), pszHistoryPath);
    if (0 >= nResult)
    {
        return FALSE;
    }

    caErr = CC_UpdateCfg(pProc->pCCWrk, pProc->szCfgFName, &cfgDatum);
    return (CA_ERR_SUCCESS == caErr ? TRUE : FALSE);
}

BOOL OCAS_SetMonStateToUI(HWND hWnd, const TCHAR *pszState)
{
    return OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_MONSTATE, 
        NULL == pszState || '\0' == pszState ? TEXT("Unknown") : pszState);
}

BOOL OCAS_OnWrkDescChange(HWND hWnd)
{
    const TCHAR *pszWrkState;
    OCAWProc *pProc = CAS_MGetProcPtr();
    if (NULL == pProc)
    {
        return FALSE;
    }
    
    EnterCriticalSection(&pProc->shellCS);
    pszWrkState = CC_StateDesc(pProc->wrkDesc.wrkState);
    LeaveCriticalSection(&pProc->shellCS);

    OCAS_SetMonStateToUI(hWnd, pszWrkState);
    OCAS_OnUpdateMonBtnState(hWnd);
    return TRUE;
}

BOOL OCAS_SetMonProcNameToUI(HWND hWnd, const TCHAR *pszFName)
{
    return OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_MONPROC, pszFName);
}

BOOL OCAS_SetHistoryPathToUI(HWND hMainWnd, const TCHAR *pszPath)
{
    return OCAS_SetDlgItemTxt(hMainWnd, IDC_EDIT_HISTORY_PATH, pszPath);
}

BOOL OCAS_BrowseHistoryPath(HWND hWnd)
{
    CACfgDatum cfgDatum;

    CA_CfgDupRT(&cfgDatum);
    ShellExecute(hWnd, TEXT("open"), cfgDatum.szHistoryPath, 
        NULL, NULL, SW_SHOWNORMAL);
    return TRUE;
}

BOOL OCAS_OnChangeHistoryPath(HWND hWnd)
{
    TCHAR szPath[MAX_PATH];
    BOOL bResult;

    bResult = OCAS_SelPath(hWnd, szPath, 
        sizeof(szPath) / sizeof(szPath[0]));
    if (!bResult)
    {
        return FALSE;
    }

    return OCAS_ChangeHistoryPath(hWnd, szPath);
}

BOOL OCAS_OnMonCtlClick(HWND hWnd)
{
    OCAWProc *pProc = CAS_MGetProcPtr();
    CAErrno caErr;
    BOOL bIsPause;

    if (NULL == pProc || NULL == pProc->pCCWrk)
    {
        return FALSE;
    }

    bIsPause = CC_IsPause(pProc->pCCWrk);
    caErr = CC_SetPauseFlag(pProc->pCCWrk, !bIsPause);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* XXX FIXME pop a err msg */
    }

    return OCAS_OnUpdateMonBtnState(hWnd);
}

BOOL OCAS_OnMainDlgInit(HWND hWnd)
{
    CACfgDatum cfgDatum;
    HICON hAppIcon; 
    
    hAppIcon = LoadIcon(CAS_MGetAppInst(), MAKEINTRESOURCE(IDI_ICON_MAIN));
    if (NULL != hAppIcon)
    {
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hAppIcon);
    }
    
    /* set value to ctls */
    CA_CfgDupRT(&cfgDatum);
    OCAS_SetHistoryPathToUI(hWnd, cfgDatum.szHistoryPath);
    OCAS_SetMonProcNameToUI(hWnd, cfgDatum.szCommunicatorFName);
    OCAS_OnUpdateMonBtnState(hWnd);
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

    case ID_POPMENU_BROWSE_HISTORY:
        return OCAS_BrowseHistoryPath(hWnd);

    case ID_POPMENU_CLOSE:
        DestroyWindow(hWnd);
        return TRUE;

    case IDC_BUTTON_BROWSE:
        return OCAS_BrowseHistoryPath(hWnd);

    case IDC_BUTTON_CHANGE:
        return OCAS_OnChangeHistoryPath(hWnd);

    case IDC_BTN_MONCTLS:
    case IDC_BTN_MONCTLP:
        return OCAS_OnMonCtlClick(hWnd);

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
        return FALSE;
    }

    OCAS_AppendMenuItems(hMenu, g_popMenuItems, 
        sizeof(g_popMenuItems) / sizeof(g_popMenuItems[0]));

    bResult = TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, 
        cursorPos.x, cursorPos.y, 0, hWnd, NULL);
    if (!bResult)
    {

    }

    DestroyMenu(hMenu);
    return TRUE;
}
