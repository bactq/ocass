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
#include "ca_ofc.h"
#include "ca_cfg.h"
#include "resource.h"

static BOOL OCAS_CfgUpdateCfgFromUI(HWND hWnd, CACfgDatum *pCfgDatum)
{
    CALogMod logMod;
    BOOL bResult;
    BOOL bState;

    /* auto inject */
    bState = FALSE;
    bResult = OCAS_GetDlgCheckBoxState(hWnd, IDC_CHK_AUTOINJECT, &bState);
    if (!bResult)
    {
        bState = FALSE;
    }
    pCfgDatum->bIsAutoInject = bState;

    /* oc proc */
    OCAS_GetDlgItemTxt(hWnd, IDC_EDIT_CPN, pCfgDatum->szCommunicatorFName, 
        sizeof(pCfgDatum->szCommunicatorFName) / 
        sizeof(pCfgDatum->szCommunicatorFName[0]));

    /* log mod */
    logMod = CA_LOGMOD_ERR;    
    bResult = OCAS_ComboBoxGetCurSelItemData(GetDlgItem(hWnd, 
        IDC_COMBO_LOGMOD), (void**)&logMod);
    if (!bResult)
    {
        logMod = CA_LOGMOD_ERR;
    }
    pCfgDatum->logMode = logMod;

    /* log file */
    OCAS_GetDlgItemTxt(hWnd, IDC_EDIT_LOGDIR, pCfgDatum->szLogPath, 
        sizeof(pCfgDatum->szLogPath) / sizeof(pCfgDatum->szLogPath[0]));
    return TRUE;
}

static BOOL OCAS_CfgOnBtnOkClick(HWND hWnd)
{
    CACfgDatum cfgDatum;
    OCAWProc *pProc = CAS_MGetProcPtr();
    BOOL bResult;
    BOOL bIsChecked;

    bIsChecked = FALSE;
    bResult = OCAS_GetDlgCheckBoxState(hWnd, 
        IDC_CHK_AUTOSTART, &bIsChecked);
    if (!bResult)
    {
        bIsChecked = FALSE;
    }
    CA_CfgOCASSAutoRunSet(bIsChecked);
    
    if (NULL == pProc || NULL == pProc->pCCWrk)
    {
        goto EXIT;
    }

    CA_CfgDupRT(&cfgDatum);
    bResult = OCAS_CfgUpdateCfgFromUI(hWnd, &cfgDatum);
    if (bResult)
    {
        CC_UpdateCfg(pProc->pCCWrk, pProc->szCfgFName, &cfgDatum);
    }

EXIT:
    SendMessage(hWnd, WM_CLOSE, NULL, NULL);
    return TRUE;
}

static BOOL OCAS_CfgOnInit(HWND hWnd)
{
    CACfgDatum cfgDatum;
    HWND hLogMod;
    BOOL bIsAutoRun;

    CA_CfgDupRT(&cfgDatum);
    OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_CPN, cfgDatum.szCommunicatorFName);
    OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_LOGDIR, cfgDatum.szLogPath);

    hLogMod = GetDlgItem(hWnd, IDC_COMBO_LOGMOD);
    if (NULL != hLogMod)
    {
        OCAS_ComboBoxDelAllItems(hLogMod);
        OCAS_ComboBoxAddItems(hLogMod, g_logModComboItems, 
            sizeof(g_logModComboItems) / sizeof(g_logModComboItems[0]));        
        OCAS_ComboBoxSetSelWithData(hLogMod, (void*)cfgDatum.logMode, 0);
    }

    OCAS_SetDlgCheckBoxState(hWnd, IDC_CHK_AUTOINJECT, 
        cfgDatum.bIsAutoInject);

    CA_CfgOCASSIsAutoRun(&bIsAutoRun);
    OCAS_SetDlgCheckBoxState(hWnd, IDC_CHK_AUTOSTART, bIsAutoRun);
    return TRUE;
}

static BOOL OCAS_CfgOnScanCPNameClick(HWND hWnd)
{
    CAErrno caErr;
    TCHAR szOCPName[MAX_PATH];

    caErr = CA_OFCGetFName(szOCPName, 
        sizeof(szOCPName) / sizeof(szOCPName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return TRUE;
    }

    return OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_CPN, szOCPName);
}

static BOOL OCAS_CfgOnLogPathBtnClick(HWND hWnd)
{
    TCHAR szLogPath[MAX_PATH];
    BOOL bResult;

    bResult = OCAS_SelPath(hWnd, szLogPath, 
        sizeof(szLogPath) / sizeof(szLogPath[0]));
    if (!bResult)
    {
        return TRUE;
    }

    return OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_LOGDIR, szLogPath);
}

static BOOL OCAS_CfgOnCPNameClick(HWND hWnd)
{
    TCHAR szFName[MAX_PATH];
    BOOL bResult;

    bResult = OCAS_SelFile(hWnd, TEXT("Programs (*.exe)\0*.exe\0"), 
        szFName, sizeof(szFName) / sizeof(szFName[0]));
    if (!bResult)
    {
        return TRUE;
    }

    return OCAS_SetDlgItemTxt(hWnd, IDC_EDIT_CPN, szFName);
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

        case IDC_BTN_SCANCPN:
            return OCAS_CfgOnScanCPNameClick(hWnd);

        case IDC_BTN_BRCPN:
            return OCAS_CfgOnCPNameClick(hWnd);

        case IDC_BTN_BRLOGDIR:
            return OCAS_CfgOnLogPathBtnClick(hWnd);
        default:
            return FALSE;
        }
        return FALSE;

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
