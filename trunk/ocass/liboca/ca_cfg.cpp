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
#include "ca_cfg.h"
#include "ca_misc.h"
#include "ca_ofc.h"
#include "ca_str.h"
#include "ca_inner.h"

static CACfgDatum g_rtCfgDatum = {0};

static CAErrno CA_CfgGetDefaultSetVals(const TCHAR *pszWrkPath, 
                                       CACfgDatum *pCfgDatum)
{
    CAErrno caErr;
    DWORD dwBufCnt;

    pCfgDatum->szCommunicatorFName[0] = '\0';
    pCfgDatum->szTemplatePath[0] = '\0';

    pCfgDatum->szHistoryPath[0] = '\0';
    pCfgDatum->szSpyLog[0] = '\0';
    pCfgDatum->szSpyNtDump[0] = '\0';
    pCfgDatum->szShellLog[0] = '\0';
    pCfgDatum->szLogPath[0] = '\0';
    pCfgDatum->bIsAutoInject = TRUE;
    pCfgDatum->logMode = CA_LOGMOD_ERR;

    pCfgDatum->spyLogMask = CA_CfgLogMod2SpyLogMask(CA_LOGMOD_ERR);
    pCfgDatum->shellLogMask = CA_CfgLogMod2ShLogMask(CA_LOGMOD_ERR);

    pCfgDatum->dwSpyLogTSize = CA_CFG_DEFAULT_SPY_LOG_TSIZE_B;
    pCfgDatum->dwSpyNtDumpTSize = CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_B;
    pCfgDatum->dwShellTSize = CA_CFG_DEFAULT_SHELL_TSIZE_B;

    /* Communicator file name */
    dwBufCnt = sizeof(pCfgDatum->szCommunicatorFName) / 
               sizeof(pCfgDatum->szCommunicatorFName[0]);
    caErr = CA_OFCGetFName(pCfgDatum->szCommunicatorFName, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* template path */
    dwBufCnt = sizeof(pCfgDatum->szTemplatePath) / 
               sizeof(pCfgDatum->szTemplatePath[0]);
    caErr = CA_PathJoin(pszWrkPath, CA_CFG_DEFAULT_TEMPLATE_PATH, 
        pCfgDatum->szTemplatePath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* history dump path */
    dwBufCnt = sizeof(pCfgDatum->szHistoryPath) / 
               sizeof(pCfgDatum->szHistoryPath[0]);
    caErr = CA_PathJoin(pszWrkPath, CA_CFG_DEFAULT_HISTORY_PATH, 
        pCfgDatum->szHistoryPath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }


    /* log path */
    dwBufCnt = sizeof(pCfgDatum->szLogPath) / 
               sizeof(pCfgDatum->szLogPath[0]);
    caErr = CA_PathJoin(pszWrkPath, CA_CFG_DEFAULT_LOG_PATH, 
        pCfgDatum->szLogPath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* spy log */
    dwBufCnt = sizeof(pCfgDatum->szSpyLog) / 
               sizeof(pCfgDatum->szSpyLog[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SPY_LOG, 
        pCfgDatum->szSpyLog, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* nt dump */
    dwBufCnt = sizeof(pCfgDatum->szSpyNtDump) / 
               sizeof(pCfgDatum->szSpyNtDump[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SPY_NT_DUMP, 
        pCfgDatum->szSpyNtDump, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* shell log */
    dwBufCnt = sizeof(pCfgDatum->szShellLog) / 
               sizeof(pCfgDatum->szShellLog[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SHELL_LOG, 
        pCfgDatum->szShellLog, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszWrkPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt)
{
    return CA_PathJoin(pszWrkPath, CA_CFG_FNAME, pszCfgFNameBuf, dwBufCnt);
}

CA_DECLARE(CAErrno) CA_CfgRd(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum)
{
    CACfgDatum cfgDefaultDatum;
    CAErrno caErr;
    TCHAR szDefaultVal[MAX_PATH];
    TCHAR *pszDefaultVal;
    DWORD dwBufCnt;
    int nVal;

    caErr = CA_CfgGetDefaultSetVals(pszWrkPath, &cfgDefaultDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pCfgDatum->szHistoryPath[0] = '\0';
    pCfgDatum->szCommunicatorFName[0] = '\0';
    pCfgDatum->szLogPath[0] = '\0';

    /* Communicator file name */
    dwBufCnt = sizeof(pCfgDatum->szCommunicatorFName) /
               sizeof(pCfgDatum->szCommunicatorFName[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("communicator_fname"), 
        cfgDefaultDatum.szCommunicatorFName, 
        pCfgDatum->szCommunicatorFName, dwBufCnt, pszCfgFName);

    /* template path */
    dwBufCnt = sizeof(pCfgDatum->szTemplatePath) /
               sizeof(pCfgDatum->szTemplatePath[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("template_path"), 
        cfgDefaultDatum.szTemplatePath, 
        pCfgDatum->szTemplatePath, dwBufCnt, pszCfgFName);

    /* history dump path */
    dwBufCnt = sizeof(pCfgDatum->szHistoryPath) / 
               sizeof(pCfgDatum->szHistoryPath[0]);
    GetPrivateProfileString(TEXT("spy"), TEXT("history_path"), 
        cfgDefaultDatum.szHistoryPath, pCfgDatum->szHistoryPath, 
        dwBufCnt, pszCfgFName);

    /* log path */
    dwBufCnt = sizeof(pCfgDatum->szLogPath) / 
               sizeof(pCfgDatum->szLogPath[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("log_path"), 
        cfgDefaultDatum.szLogPath, pCfgDatum->szLogPath, 
        dwBufCnt, pszCfgFName);

    /* log mode */
    pCfgDatum->logMode = GetPrivateProfileInt(TEXT("app"), 
        TEXT("log_mode"), cfgDefaultDatum.logMode, 
        pszCfgFName);

    /* is auto inject */
    pCfgDatum->bIsAutoInject = GetPrivateProfileInt(TEXT("app"), 
        TEXT("is_auto_inject"), cfgDefaultDatum.bIsAutoInject, 
        pszCfgFName);

    
    /* spy log size, mask, name  */
    nVal = GetPrivateProfileInt(TEXT("spy"), TEXT("spy_log_ts"), 
        CA_CFG_DEFAULT_SPY_LOG_TSIZE_M, pszCfgFName);
    pCfgDatum->dwSpyLogTSize = (nVal * 1024 * 1024);

    pCfgDatum->spyLogMask = GetPrivateProfileInt(TEXT("spy"), 
        TEXT("spy_logmask"), cfgDefaultDatum.spyLogMask, pszCfgFName);

    szDefaultVal[0] = '\0';
    dwBufCnt = sizeof(szDefaultVal) / sizeof(szDefaultVal[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SPY_LOG, 
        szDefaultVal, dwBufCnt);
    pszDefaultVal = (CA_ERR_SUCCESS == caErr ? 
        szDefaultVal : cfgDefaultDatum.szSpyLog);
    dwBufCnt = sizeof(pCfgDatum->szLogPath) / 
               sizeof(pCfgDatum->szLogPath[0]);
    GetPrivateProfileString(TEXT("spy"), TEXT("spy_log"), pszDefaultVal, 
        pCfgDatum->szSpyLog, dwBufCnt, pszCfgFName);

    /* spy net log size , name */
    nVal = GetPrivateProfileInt(TEXT("spy"), TEXT("spy_ntdump_ts"), 
        CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_M, pszCfgFName);
    pCfgDatum->dwSpyNtDumpTSize = (nVal * 1024 * 1024);

    szDefaultVal[0] = '\0';
    dwBufCnt = sizeof(szDefaultVal) / sizeof(szDefaultVal[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SPY_NT_DUMP, 
        szDefaultVal, dwBufCnt);
    pszDefaultVal = (CA_ERR_SUCCESS == caErr ? 
        szDefaultVal : cfgDefaultDatum.szSpyNtDump);
    dwBufCnt = sizeof(pCfgDatum->szSpyNtDump) / 
               sizeof(pCfgDatum->szSpyNtDump[0]);
    GetPrivateProfileString(TEXT("spy"), TEXT("spy_ntdump"), pszDefaultVal, 
        pCfgDatum->szSpyNtDump, dwBufCnt, pszCfgFName);

    /* shell log size, mask, name */
    nVal = GetPrivateProfileInt(TEXT("app"), TEXT("log_ts"), 
        CA_CFG_DEFAULT_SHELL_TSIZE_M, pszCfgFName);
    pCfgDatum->dwShellTSize = (nVal * 1024 * 1024);
    pCfgDatum->shellLogMask = GetPrivateProfileInt(TEXT("app"), 
        TEXT("log_mask"), cfgDefaultDatum.shellLogMask, pszCfgFName);

    szDefaultVal[0] = '\0';
    dwBufCnt = sizeof(szDefaultVal) / sizeof(szDefaultVal[0]);
    caErr = CA_PathJoin(pCfgDatum->szLogPath, CA_CFG_DEFAULT_SHELL_LOG, 
        szDefaultVal, dwBufCnt);
    pszDefaultVal = (CA_ERR_SUCCESS == caErr ? 
        szDefaultVal : cfgDefaultDatum.szShellLog);
    dwBufCnt = sizeof(pCfgDatum->szShellLog) / 
               sizeof(pCfgDatum->szShellLog[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("log_fname"), pszDefaultVal, 
        pCfgDatum->szShellLog, dwBufCnt, pszCfgFName);

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CACfgDatum*) CA_CfgGetRT(void)
{
    return &g_rtCfgDatum;
}

CA_DECLARE(CAErrno) CA_CfgSetRT(CACfgDatum *pCfgDatum)
{
    CA_RTCSEnter();
    memcpy(&g_rtCfgDatum, pCfgDatum, sizeof(g_rtCfgDatum));
    CA_RTCSLeave();
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgSetRTDefault(void)
{        
    CAErrno funcErr = CA_ERR_SUCCESS;

    CA_RTCSEnter();
    funcErr = CA_CfgGetDefaultSetVals(CA_GetPtrRT()->szRTWrkPath, 
        &g_rtCfgDatum);
    CA_RTCSLeave();
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CfgSetRTFromFile(const TCHAR *pszCfgFName, 
                                        const TCHAR *pszWrkPath)
{
    CACfgDatum cfgDatum = {0};
    CAErrno caErr;

    caErr = CA_CfgRd(pszCfgFName, pszWrkPath, &cfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    return CA_CfgSetRT(&cfgDatum);
}

CA_DECLARE(CAErrno) CA_CfgDupRT(CACfgDatum *pResult)
{
    CA_RTCSEnter(); 
    memcpy(pResult, CA_CfgGetRT(), sizeof(CACfgDatum));
    CA_RTCSLeave();
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgOCASSIsAutoRun(BOOL *pbIsAutoRun)
{
    TCHAR szOCASSProc[MAX_PATH];
    DWORD dwDataLen;
    HKEY hKeyAutoRun = NULL;    
    LONG nResult;

    *pbIsAutoRun = FALSE;
    nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 
        0, KEY_READ, &hKeyAutoRun);
    if (ERROR_SUCCESS != nResult)
    {
        return CA_ERR_SUCCESS;
    }

    dwDataLen = sizeof(szOCASSProc) / sizeof(szOCASSProc[0]);
    nResult = RegQueryValueEx(hKeyAutoRun, CA_CFG_REG_RUN_KEY, 
        NULL, NULL, (LPBYTE)szOCASSProc, &dwDataLen);
    if (ERROR_SUCCESS != nResult)
    {
        goto EXIT;
    }

    /* XXX FIXME we need compare the process name ??? */
    *pbIsAutoRun = TRUE;
EXIT:
    if (NULL != hKeyAutoRun)
    {
        RegCloseKey(hKeyAutoRun);
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgOCASSAutoRunSet(BOOL bIsAutoRun)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    TCHAR szOCASSProc[MAX_PATH];
    DWORD dwResult;
    HKEY hKeyAutoRun = NULL;    
    LONG nResult;

    szOCASSProc[0] = '\0';
    dwResult = GetModuleFileName(NULL, szOCASSProc, 
        sizeof(szOCASSProc) / sizeof(szOCASSProc[0]));
    if (0 >= dwResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    nResult =  RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 
        0, KEY_ALL_ACCESS, &hKeyAutoRun);
    if (ERROR_SUCCESS != nResult)
    {
        return CA_ERR_SUCCESS;
    }


    nResult = RegDeleteValue(hKeyAutoRun, CA_CFG_REG_RUN_KEY);
    if (ERROR_SUCCESS != nResult)
    {
        funcErr = CA_ERR_SYS_CALL;
    }

    if (bIsAutoRun)
    {
        nResult =  RegSetValueEx(hKeyAutoRun, 
            TEXT("ocass_autorun"), 
            0, REG_SZ, (LPBYTE)szOCASSProc, 
            lstrlen(szOCASSProc) * sizeof(szOCASSProc[0]));
        if (ERROR_SUCCESS != nResult)
        {
            funcErr = CA_ERR_SYS_CALL;
            goto EXIT;
        }
        else
        {
            funcErr = ERROR_SUCCESS;
        }
    }

EXIT:
    if (NULL != hKeyAutoRun)
    {
        RegCloseKey(hKeyAutoRun);
    }

    return funcErr;
}

CA_DECLARE(CASpyLogMask) CA_CfgLogMod2SpyLogMask(CALogMod logMod)
{
    switch (logMod)
    {
    case CA_LOGMOD_WARN:
        return CA_SPY_LOG_WARN|CA_SPY_LOG_ERR|CA_SPY_LOG_DEL_OLD|
            CA_SPY_LOG_NT_DUMP;

    case CA_LOGMOD_NONE:
        return CA_SPY_LOG_NONE;

    case CA_LOGMOD_DBG:
        return CA_SPY_LOG_INFO|CA_SPY_LOG_DBG|CA_SPY_LOG_WARN|
            CA_SPY_LOG_ERR|CA_SPY_LOG_NT_ADUMP|CA_SPY_LOG_DEL_OLD;

    case CA_LOGMOD_DBG_ARC:
        return CA_SPY_LOG_INFO|CA_SPY_LOG_DBG|CA_SPY_LOG_WARN|
            CA_SPY_LOG_ERR|CA_SPY_LOG_NT_ADUMP|CA_SPY_LOG_RENAME_OLD;

    default:
    case CA_LOGMOD_ERR:
        return CA_SPY_LOG_ERR|CA_SPY_LOG_DEL_OLD;
    }
}

CA_DECLARE(CAShellLogMask) CA_CfgLogMod2ShLogMask(CALogMod logMod)
{
    switch (logMod)
    {
    case CA_LOGMOD_WARN:
        return CA_SH_LOG_WARN|CA_SH_LOG_ERR|CA_SH_LOG_DEL_OLD;

    case CA_LOGMOD_NONE:
        return CA_SH_LOG_NONE;

    case CA_LOGMOD_DBG:
        return CA_SH_LOG_DBG|CA_SH_LOG_INFO|CA_SH_LOG_WARN|
            CA_SH_LOG_ERR|CA_SH_LOG_DEL_OLD;


    case CA_LOGMOD_DBG_ARC:
        return CA_SH_LOG_DBG|CA_SH_LOG_INFO|CA_SH_LOG_WARN|
            CA_SH_LOG_ERR|CA_SH_LOG_RENAME_OLD;

    default:
    case CA_LOGMOD_ERR:
        return CA_SH_LOG_ERR|CA_SH_LOG_DEL_OLD;
    }
}

CA_DECLARE(CAErrno) CA_CfgShWr(const TCHAR *pszCfgFName, 
                               CACfgDatum *pCfgDatum)
{
    /* XXX FIXME if the path is not FULL */
    WritePrivateProfileString(TEXT("app"), TEXT("communicator_fname"), 
        pCfgDatum->szCommunicatorFName, pszCfgFName);

    WritePrivateProfileString(TEXT("app"), TEXT("history_path"), 
        pCfgDatum->szHistoryPath, pszCfgFName);

    /* XXX XXX */
    return CA_ERR_SUCCESS;
}
