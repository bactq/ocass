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

#include "ca_types.h"
#include "ca_misc.h"
#include "ca_mod.h"
#include "ca_cfg.h"
#include "liboch.h"
#include "ch_inner.h"

typedef CAErrno (*CS_EntryFunc)(HMODULE hLib, 
                                CACfgDatum *pCfgDatum);

void CH_OnInject(CHHDatum *pHDatum)
{
    CS_EntryFunc pEntryFunc;
    HMODULE hModSpy;
    CAErrno caErr;
    TCHAR szModPath[MAX_PATH];
    TCHAR szModSpy[MAX_PATH];
    TCHAR *pSlash;
    DWORD dwResult;
    int nResult;

    dwResult = GetModuleFileName(CH_GetDllInstance(), szModPath, 
        sizeof(szModPath) / sizeof(szModPath[0]));
    if (0 == dwResult)
    {
        return;
    }
    pSlash = strrchr(szModPath, '\\');
    if (NULL == pSlash)
    {
        return;
    }

    pSlash[0] = '\0';
    nResult = _snprintf(szModSpy, sizeof(szModSpy) / sizeof(szModSpy[0]), 
        TEXT("%s\\%s"), szModPath, CA_MOD_OCS);
    if (0 >= nResult)
    {
        return;
    }

    hModSpy = LoadLibrary(szModSpy);
    if (NULL == hModSpy)
    {
        return;
    }

    pEntryFunc = (CS_EntryFunc)GetProcAddress(hModSpy, TEXT("CS_Entry"));
    if (NULL == pEntryFunc)
    {
        FreeLibrary(hModSpy);
        return;
    }

    caErr = pEntryFunc(hModSpy, &(pHDatum->cfgDatum));
    if (CA_ERR_SUCCESS != caErr)
    {
        FreeLibrary(hModSpy);
        return;
    }
}

void CH_OnInjectComplete(CHHDatum *pHDatum)
{
    if (NULL == pHDatum || NULL == pHDatum->hInjectWndHook)
    {
        return;
    }

    /* unhook this proc */
    UnhookWindowsHookEx(pHDatum->hInjectWndHook);
    pHDatum->hInjectWndHook = NULL;
}

CA_DECLARE(CAErrno) CH_PInject(DWORD dwProcId, DWORD dwTimeOut)
{
    CHHDatum *pHDatum = CH_HDatumPtrGet();
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    DWORD dwWaitTime;
    DWORD dwThId;

    caErr = CA_GetProcFirstThread(dwProcId, &dwThId);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* XXX log */
        return caErr;
    }

    /* we need wait the Communicator proc startuped */
    Sleep(1000 * 5);

    CA_CfgDupRT(&(pHDatum->cfgDatum));
    pHDatum->hInjectWndHook = SetWindowsHookEx(WH_CBT, 
            (HOOKPROC)CH_HookCBTProc, CH_GetDllInstance(), dwThId);
    if (NULL == pHDatum->hInjectWndHook)
    {
        return CA_ERR_SYS_CALL;
    }

    PostThreadMessage(dwThId, WM_NULL, 0, 0);

    /* wait for inject success */
    for (dwWaitTime = 0;;)
    {
        if (NULL == pHDatum->hInjectWndHook)
        {
            break;
        }

        Sleep(500);
        dwWaitTime += 500;
        if (0 != dwTimeOut && dwWaitTime < dwTimeOut )
        {
            funcErr = CA_ERR_TIMEOUT;
            break;
        }
    }

    return funcErr;
}
