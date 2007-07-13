/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
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
                               const CACfgDatum *pOldCfgDatum, 
                               CACfgDatum *pCfgDatum)
{
    TCHAR szVal[128];
    int nResult;

    /* log path */
    nResult = lstrcmpi(pOldCfgDatum->szLogPath, pCfgDatum->szLogPath);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("app"), TEXT("log_path"), 
            pCfgDatum->szLogPath, pszCfgFName);
    }

    /* log mode */
    if (pOldCfgDatum->logMode != pCfgDatum->logMode)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->logMode);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("app"), TEXT("log_mode"), 
                szVal, pszCfgFName);
        }
    }

    /* is auto inject */
    if (pOldCfgDatum->bIsAutoInject != pCfgDatum->bIsAutoInject)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->bIsAutoInject);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("app"), TEXT("is_auto_inject"), 
                szVal, pszCfgFName);
        }
    }

    /* oc fname */
    nResult = lstrcmpi(pOldCfgDatum->szCommunicatorFName, 
        pCfgDatum->szCommunicatorFName);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("app"), TEXT("communicator_fname"), 
            pCfgDatum->szCommunicatorFName, pszCfgFName);
    }

    /* history path */
    nResult = lstrcmpi(pOldCfgDatum->szHistoryPath, 
        pCfgDatum->szHistoryPath);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("spy"), TEXT("history_path"), 
            pCfgDatum->szHistoryPath, pszCfgFName);
    }

    /* template path */
    nResult = lstrcmpi(pOldCfgDatum->szTemplatePath, 
        pCfgDatum->szTemplatePath);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("app"), TEXT("template_path"), 
            pCfgDatum->szTemplatePath, pszCfgFName);
    }

    /* spy log */
    nResult = lstrcmpi(pOldCfgDatum->szSpyLog, 
        pCfgDatum->szSpyLog);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("spy"), TEXT("spy_log"), 
            pCfgDatum->szSpyLog, pszCfgFName);
    }

    /* spy log size */
    if (pOldCfgDatum->dwSpyLogTSize != pCfgDatum->dwSpyLogTSize)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->dwSpyLogTSize);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("spy"), TEXT("spy_log_ts"), 
                szVal, pszCfgFName);
        }
    }

    /* nt dump log */
    nResult = lstrcmpi(pOldCfgDatum->szSpyNtDump, 
        pCfgDatum->szSpyNtDump);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("spy"), TEXT("spy_ntdump"), 
            pCfgDatum->szSpyNtDump, pszCfgFName);
    }

    /* nt dump log size */
    if (pOldCfgDatum->dwSpyNtDumpTSize != pCfgDatum->dwSpyNtDumpTSize)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->dwSpyNtDumpTSize);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("spy"), TEXT("spy_ntdump_ts"), 
                szVal, pszCfgFName);
        }
    }

    /* shell log */
    nResult = lstrcmpi(pOldCfgDatum->szShellLog, 
        pCfgDatum->szShellLog);
    if (0 != nResult)
    {
        WritePrivateProfileString(TEXT("app"), TEXT("log_fname"), 
            pCfgDatum->szShellLog, pszCfgFName);
    }

    /* shell log size */
    if (pOldCfgDatum->dwShellTSize != pCfgDatum->dwShellTSize)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->dwShellTSize);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("app"), TEXT("log_ts"), 
                szVal, pszCfgFName);
        }
    }

    /* shell log mask */
    if (pOldCfgDatum->shellLogMask != pCfgDatum->shellLogMask)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->shellLogMask);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("app"), TEXT("log_mask"), 
                szVal, pszCfgFName);
        }
    }

    /* spy log mask */
    if (pOldCfgDatum->spyLogMask != pCfgDatum->spyLogMask)
    {
        szVal[0] = '\0';
        nResult = CA_SNPrintf(szVal, sizeof(szVal) / sizeof(szVal[0]), 
                TEXT("%u"), pCfgDatum->spyLogMask);
        if (0 < nResult)
        {
            WritePrivateProfileString(TEXT("spy"), TEXT("spy_logmask"), 
                szVal, pszCfgFName);
        }
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgReCalculate(CACfgDatum *pResult)
{
    CAErrno caErr;

    pResult->shellLogMask = CA_CfgLogMod2ShLogMask(pResult->logMode);
    pResult->spyLogMask = CA_CfgLogMod2SpyLogMask(pResult->logMode);
    caErr = CA_PathJoin(pResult->szLogPath, 
        CA_CFG_DEFAULT_SPY_LOG, pResult->szSpyLog, 
        sizeof(pResult->szSpyLog) / sizeof(pResult->szSpyLog[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_PathJoin(pResult->szLogPath, 
        CA_CFG_DEFAULT_SPY_NT_DUMP, pResult->szSpyNtDump, 
        sizeof(pResult->szSpyNtDump) / sizeof(pResult->szSpyNtDump[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_PathJoin(pResult->szLogPath, 
        CA_CFG_DEFAULT_SHELL_LOG, pResult->szShellLog, 
        sizeof(pResult->szShellLog) / sizeof(pResult->szShellLog[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    return CA_ERR_SUCCESS;
}
