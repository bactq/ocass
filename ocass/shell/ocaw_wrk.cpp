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

#include "ca_evts.h"
#include "ocaw_main.h"
#include "ocaw_proc.h"
#include "ocaw_wrk.h"
#include "ocaw_evts.h"
#include "ocaw_panic.h"
#include "resource.h"

static DWORD WINAPI OCAS_ShWrkTh(VOID *pArg)
{
    CCWrkStateDesc wrkState;
    OCAWProc *pProc = (OCAWProc *)pArg;
    CAErrno caErr;
    DWORD dwWrkCycle = 1000 * 2;
    DWORD dwResult;

    for (;;)
    {
        dwResult = WaitForSingleObject(pProc->hShWrkEvt, dwWrkCycle);
        if (pProc->bWrkStop)
        {
            break;
        }

        if (WAIT_FAILED == dwResult)
        {
            Sleep(1000);
            continue;
        }
        if (WAIT_TIMEOUT == dwResult)
        {
            caErr = CC_State(pProc->pCCWrk, TRUE, &wrkState);
            if (CA_ERR_SUCCESS != caErr)
            {
                /* XXX write log */
                continue;
            }

            if (!wrkState.bIsDirty)
            {
                continue;
            }
            
            EnterCriticalSection(&pProc->shellCS);
            memcpy(&pProc->wrkDesc, &wrkState, sizeof(wrkState));
            LeaveCriticalSection(&pProc->shellCS);

            if (NULL != pProc->hMainDlg)
            {
                SendMessage(pProc->hMainDlg, OCAW_MSG_STATE_CHANGE, 
                    NULL, NULL);
            }
            continue;
        }

        /* send msg to main dlg */
        if (NULL != pProc->hMainDlg)
        {
            SendMessage(pProc->hMainDlg, OCAW_MSG_WAKEUP, NULL, NULL);
        }
    }

    return CA_THREAD_EXIT_OK;
}

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

    case OCAW_MSG_WAKEUP:
        return OCAS_MainDlgShow(hWnd);

    case WM_CLOSE:
        return OCAS_OnMainDlgClose(hWnd);

    case OCAW_MSG_STATE_CHANGE:
        return OCAS_OnWrkDescChange(hWnd);

    default:
        return FALSE;
    }
}

int OCAS_PWrk(OCAWProc *pProc)
{
    NOTIFYICONDATA notifyIconData;
    CAErrno caErr;
    HANDLE hEvt;
    DWORD dwThId;
    BOOL bResult;
    MSG  wndMsg;
    int nProcExit = CA_PROC_EXIT_OK;

    pProc->hMainDlg     = NULL;
    pProc->hShWrkEvt    = NULL;
    pProc->hShWrkTh     = NULL;
    pProc->bWrkStop     = FALSE;
    pProc->pCCWrk       = NULL;
    pProc->wrkDesc.bIsDirty = FALSE;
    pProc->wrkDesc.startTime = time(NULL);
    pProc->wrkDesc.wrkMod = CC_WRK_MOD_PAUSE;
    pProc->wrkDesc.wrkState = CC_WRK_STATE_IDLE;

    /* create wrk evt */
    hEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, OCASS_EVT_NAME_SHELL_WRK);
    if (NULL != hEvt)
    {
        CloseHandle(hEvt);

        /* XXX panic */
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    pProc->hShWrkEvt = CreateEvent(NULL, FALSE, FALSE, 
        OCASS_EVT_NAME_SHELL_WRK);
    if (NULL == pProc->hShWrkEvt)
    {
        /* XXX panic */
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    caErr = CC_Startup(CC_WRK_MOD_SAFE, &pProc->pCCWrk);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* panic */
        DestroyWindow(pProc->hMainDlg);
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    /* create wrk thread */
    pProc->hShWrkTh = CreateThread(NULL, 0, OCAS_ShWrkTh, pProc, 0, &dwThId);
    if (NULL == pProc->hShWrkTh)
    {
        /* XXX panic */
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    pProc->hMainDlg = CreateDialog(CAS_MGetAppInst(), 
        MAKEINTRESOURCE(IDD_DLG_MAIN), NULL, OCAS_MainDlgProc);
    if (NULL == pProc->hMainDlg)
    {
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Create Main dialog failed. System Last Error is %u"), 
            GetLastError());
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    if (!pProc->bIsBackground)
    {
        ShowWindow(pProc->hMainDlg, SW_SHOWNORMAL);
        SetForegroundWindow(pProc->hMainDlg);
    }

    /* add notify icon */
    notifyIconData.cbSize = sizeof(notifyIconData);
    notifyIconData.hIcon = LoadIcon(CAS_MGetAppInst(), 
        MAKEINTRESOURCE(IDI_ICON_MAIN));
    notifyIconData.hWnd = pProc->hMainDlg;
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

        DestroyWindow(pProc->hMainDlg);
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    if (!pProc->bIsSafeMod)
    {
        CC_SetWrkMod(pProc->pCCWrk, CC_WRK_MOD_NORMAL);
    }

    for (;;)
    {
        bResult = GetMessage(&wndMsg, 0, 0, 0);
        if (!bResult)
        {
            break;
        }

        bResult = IsDialogMessage(pProc->hMainDlg, &wndMsg);
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

    pProc->hMainDlg = NULL;
EXIT:
    pProc->bWrkStop = TRUE;
    if (NULL != pProc->hShWrkTh)
    {
        SetEvent(pProc->hShWrkEvt);
        WaitForSingleObject(pProc->hShWrkTh, INFINITE);
        CloseHandle(pProc->hShWrkTh);
        pProc->hShWrkTh = NULL;
    }

    if (NULL != pProc->pCCWrk)
    {
        CC_Cleanup(pProc->pCCWrk);
        pProc->pCCWrk = NULL;
    }

    if (NULL != pProc->hShWrkEvt)
    {
        CloseHandle(pProc->hShWrkEvt);
        pProc->hShWrkEvt = NULL;
    }
    return nProcExit;
}
