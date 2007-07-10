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
#include "ca_log.h"
#include "ca_cfg.h"
#include "ca_misc.h"
#include "ocaw_log.h"

typedef struct _CASLog
{
    CAShellLogMask shellLogMask;
    CALog *pLog;
} CASLog;

static CASLog g_casLog = {0};
static CASLog *g_pCASLog = NULL;

static CASLog* CAS_GetLogDataPtr(void)
{
    return g_pCASLog;
}

static BOOL CAS_LogNeedFilter(CAShellLogMask shellLogMask, 
                              CARTLogFlags logFlags)
{
    if (CA_SH_LOG_NONE == shellLogMask)
    {
        return TRUE;
    }

    else if (CA_SH_LOG_DBG &  shellLogMask)
    {
        /* if log level is dbg then we need all log */
        return FALSE;
    }
    else if ((CA_SH_LOG_INFO & shellLogMask) && 
        (!(logFlags & CA_RTLOG_ERR) && !(logFlags & CA_RTLOG_WARN) && 
         !(logFlags & CA_RTLOG_INFO)))
    {
        return TRUE;
    }
    else if ((CA_SH_LOG_WARN & shellLogMask) && 
        (!(logFlags & CA_RTLOG_ERR) && !(logFlags & CA_RTLOG_WARN)))
    {
        return TRUE;
    }
    else if ((CA_SH_LOG_ERR & shellLogMask) &&
        !(logFlags & CA_RTLOG_ERR))
    {
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

static void CAS_RTLog(void *pCbCtx, CARTLog *pLog)
{
    CAErrno caErr;
    CASLog *pCASLog = CAS_GetLogDataPtr();
    TCHAR szTmBuf[CA_TM_STR_MAX_BUF];
    BOOL bResult;

    if (NULL == pCASLog || NULL == pCASLog->pLog)
    {
        return;
    }

    bResult = CAS_LogNeedFilter(pCASLog->shellLogMask, 
        pLog->logFlags);
    if (bResult)
    {
        return;
    }

    caErr = CA_GetCurTimeStr(szTmBuf, sizeof(szTmBuf) / sizeof(szTmBuf[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTmBuf[0] = '\0';
    }

    CA_LogLine(pCASLog->pLog, TEXT("%s %s %s(%u): %s"), 
        szTmBuf, CA_RTLogFlagsDesc(pLog->logFlags), 
        pLog->pszSrcBase, pLog->nSrcLine, pLog->pszLog);
}

CAErrno CAS_LogStartup(void)
{
    CACfgDatum cfgDatum;
    CALogOpt logOpt;
    CAErrno caErr;
    CALog *pLog;

    if (NULL != g_pCASLog)
    {
        return CA_ERR_BAD_SEQ;
    }

    caErr = CA_CfgDupRT(&cfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    logOpt.pszLogFName = cfgDatum.szShellLog;
    logOpt.dwTruncateSize = cfgDatum.dwShellTSize;
    logOpt.logAct = (cfgDatum.shellLogMask & CA_SH_LOG_RENAME_OLD ? 
        CA_LOG_TRUNCATE_RNAME : CA_LOG_TRUNCATE_DEL);
    caErr = CA_LogOpen(&logOpt, &pLog);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    g_casLog.pLog = pLog;
    g_casLog.shellLogMask = cfgDatum.shellLogMask;
    g_pCASLog = &g_casLog;
    return CA_ERR_SUCCESS;
}

void CAS_LogCleanup(void)
{
    if (NULL == g_pCASLog)
    {
        return;
    }
    g_pCASLog = NULL;

    CA_LogClose(g_casLog.pLog);
    g_casLog.pLog = NULL;
    g_casLog.shellLogMask = CA_SH_LOG_NONE;
}

CAErrno CAS_UpdateCfg(void)
{
    CACfgDatum cfgDatum;
    CALogOpt logOpt;
    CAErrno caErr;
    CASLog *pCASLog = CAS_GetLogDataPtr();

    if (NULL == pCASLog || NULL == pCASLog->pLog)
    {
        return CA_ERR_BAD_SEQ;
    }

    caErr = CA_CfgDupRT(&cfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* FIXME this mask value is not thread safe */
    pCASLog->shellLogMask = cfgDatum.shellLogMask;
    logOpt.pszLogFName = cfgDatum.szShellLog;
    logOpt.dwTruncateSize = cfgDatum.dwShellTSize;
    logOpt.logAct = (cfgDatum.shellLogMask & CA_SH_LOG_RENAME_OLD ? 
        CA_LOG_TRUNCATE_RNAME : CA_LOG_TRUNCATE_DEL);
    return CA_LogUpdate(pCASLog->pLog, &logOpt);
}
