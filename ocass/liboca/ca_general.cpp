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

#include "liboca.h"
#include "ca_inner.h"
#include "ca_cfg.h"
#include "ca_str.h"
#include "ca_misc.h"

static CART g_caRT = {0};
static CART *g_pCART = NULL;

CART* CA_GetPtrRT(void)
{
    return g_pCART;
}

static CAErrno CA_RTStartup(void)
{
    CRITICAL_SECTION *pRTCS = NULL;
    HRESULT hResult;
    CAFLock *pRTPL = NULL;
    CAErrno caErr;
    CAErrno funcErr = CA_ERR_SUCCESS;
    DWORD dwBufCnt;
    TCHAR szRTFLock[MAX_PATH];

    g_caRT.hCADll = CA_GetDllInstance();
    dwBufCnt = sizeof(g_caRT.szRTWrkPath) / sizeof(g_caRT.szRTWrkPath[0]);
    caErr = CA_GetModPath(g_caRT.hCADll, g_caRT.szRTWrkPath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    dwBufCnt = sizeof(szRTFLock) / sizeof(szRTFLock[0]);
    caErr = CA_PathJoin(g_caRT.szRTWrkPath, CA_RT_PL_FNAME, 
        szRTFLock, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    InitializeCriticalSection(&g_caRT.rtCS);
    pRTCS = &g_caRT.rtCS;

    caErr = CA_FLockCreate(szRTFLock, &g_caRT.rtPL);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pRTPL = &g_caRT.rtPL;

    hResult = CoInitialize(NULL);
    if (FAILED(hResult))
    {
        funcErr = CA_ERR_SYS_CALL;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS == funcErr)
    {
        g_pCART = &g_caRT;
    }
    else
    {
        g_caRT.hCADll = NULL;
        g_caRT.szRTWrkPath[0] = '\0';

        if (NULL != pRTCS)
        {
            DeleteCriticalSection(pRTCS);
        }

        if (NULL != pRTPL)
        {
            CA_FLockDestroy(pRTPL, FALSE);
        }
    }

    return funcErr;
}

static CAErrno CA_RTCleanup(void)
{
    g_pCART = NULL;

    g_caRT.hCADll = NULL;
    g_caRT.szRTWrkPath[0] = '\0';
    DeleteCriticalSection(&g_caRT.rtCS);
    CA_FLockDestroy(&g_caRT.rtPL, FALSE);
    CoUninitialize();
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_Startup(void)
{
    CAErrno caErr;

    caErr = CA_RTStartup();
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* set default config */
    caErr = CA_CfgSetRTDefault();
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTCleanup();
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CA_Cleanup(void)
{
    CA_RTCleanup();
}

CA_DECLARE(CAErrno) CA_RTCSEnter(void)
{
    EnterCriticalSection(&(CA_GetPtrRT()->rtCS));
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_RTCSLeave(void)
{
    LeaveCriticalSection(&(CA_GetPtrRT()->rtCS));
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_RTPLockEnter(CAFLockOpt lockOpt)
{
    return CA_FLockEnter(&(CA_GetPtrRT()->rtPL), lockOpt);
}

CA_DECLARE(CAErrno) CA_RTPLockLeave(void)
{
    return CA_FLockLeave(&(CA_GetPtrRT()->rtPL));
}

CA_DECLARE(CAErrno) CA_RTSetLog(void *pCbCtx, CA_RTLogFunc pLogFunc)
{
    CA_RTCSEnter();
    CA_GetPtrRT()->pRTLogCbCtx = pCbCtx;
    CA_GetPtrRT()->pRTLogFunc = pLogFunc;
    CA_RTCSLeave();
    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CA_RTLog(const TCHAR *pszSrc, int nSrcLine, 
                          CARTLogFlags logFlags, const TCHAR *pszFmt, ...)
{
    CA_RTLogFunc pLogFunc;
    va_list pArgList;
    CARTLog rtLog;
    TCHAR szRTLog[1024 * 4];
    void *pCbCtx;
    int nResult;

    pCbCtx = CA_GetPtrRT()->pRTLogCbCtx;
    pLogFunc = CA_GetPtrRT()->pRTLogFunc;
    if (NULL == pLogFunc)
    {
        return;
    }

    va_start(pArgList, pszFmt);
    nResult = CA_VSNPrintf(szRTLog, sizeof(szRTLog) / sizeof(szRTLog[0]), 
        pszFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        return;
    }

    rtLog.logFlags = logFlags;
    rtLog.pszSrc = pszSrc;
    CA_PathGetBaseName(pszSrc, &rtLog.pszSrcBase);
    rtLog.nSrcLine = nSrcLine;
    rtLog.pszLog = szRTLog;
    pLogFunc(pCbCtx, &rtLog);
}

CA_DECLARE(const TCHAR*) CA_RTLogFlagsDesc(CARTLogFlags logFlags)
{
    if (CA_RTLOG_DBG & logFlags)
    {
        return TEXT("DBG");
    }
    else if (CA_RTLOG_WARN & logFlags)
    {
        return TEXT("WARN");
    }
    else if (CA_RTLOG_INFO & logFlags)
    {
        return TEXT("INFO");
    }
    else
    {
        return TEXT("ERR");
    }
}
