/**
 *
 */

#include "ca_str.h"
#include "ca_log.h"
#include "cs_log.h"
#include "ca_misc.h"

typedef struct _CSLog
{
    CALog *pSpyLog;
    CALog *pNtDumpLog;

    CASpyLogMask spyLogMask;
} CSLog;

static CSLog g_csLog = {0};
static CSLog *g_pCSLog = NULL;

static CSLog* CS_LogGetPtr(void)
{
    return g_pCSLog;
}

static BOOL CS_LogFalgsNeedFilter(CARTLogFlags logFlags, 
                                  CASpyLogMask spyLogMask)
{
    if (CA_SPY_LOG_NONE == spyLogMask)
    {
        return TRUE;
    }

    else if (CA_SPY_LOG_DBG &  spyLogMask)
    {
        /* if log level is dbg then we need all log */
        return FALSE;
    }
    else if ((CA_SPY_LOG_INFO & spyLogMask) && 
        (!(logFlags & CA_RTLOG_ERR) && !(logFlags & CA_RTLOG_WARN) && 
         !(logFlags & CA_RTLOG_INFO)))
    {
        return TRUE;
    }
    else if ((CA_SPY_LOG_WARN & spyLogMask) && 
        (!(logFlags & CA_RTLOG_ERR) && !(logFlags & CA_RTLOG_WARN)))
    {
        return TRUE;
    }
    else if ((CA_SPY_LOG_ERR & spyLogMask) &&
        !(logFlags & CA_RTLOG_ERR))
    {
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

CAErrno CS_LogStartup(CSLogCfg *pCSLogCfg)
{
    CALogTAct logAct;
    CALogOpt logOpt;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;

    if (NULL != g_pCSLog)
    {
        return CA_ERR_SUCCESS;
    }

    logAct = (pCSLogCfg->spyLogMask & CA_SPY_LOG_RENAME_OLD ?
        CA_LOG_TRUNCATE_RNAME : CA_LOG_TRUNCATE_DEL);

    logOpt.pszLogFName = pCSLogCfg->pszSpyLogFName;
    logOpt.logAct = logAct;
    logOpt.dwTruncateSize = pCSLogCfg->dwSpyLogTSize;
    caErr = CA_LogOpen(&logOpt, &g_csLog.pSpyLog);
    if (CA_ERR_SUCCESS != caErr)
    {
        g_csLog.pSpyLog = NULL;
    }

    logOpt.pszLogFName = pCSLogCfg->pszNtDumpLogFName;
    logOpt.logAct = logAct;
    logOpt.dwTruncateSize = pCSLogCfg->dwNtDumpLogTSize;
    caErr = CA_LogOpen(&logOpt, &g_csLog.pNtDumpLog);
    if (CA_ERR_SUCCESS != caErr)
    {
        g_csLog.pNtDumpLog = NULL;
    }

    g_pCSLog = &g_csLog;
    return funcErr;
}

void CS_LogCleanup(void)
{
    if (NULL == g_pCSLog)
    {
        return;
    }

    g_pCSLog = NULL;

    CA_LogClose(g_csLog.pNtDumpLog);
    g_csLog.pNtDumpLog = NULL;

    CA_LogClose(g_csLog.pSpyLog);
    g_csLog.pSpyLog = NULL;
}

CAErrno CS_LogUpdateCfg(CSLogCfg *pCSLogCfg)
{
    CALogTAct logAct;
    CALogOpt logOpt;
    CSLog *pCSLog = CS_LogGetPtr();

    if (NULL == pCSLog)
    {
        return CA_ERR_BAD_SEQ;
    }

    logAct = (pCSLogCfg->spyLogMask & CA_SPY_LOG_RENAME_OLD ?
        CA_LOG_TRUNCATE_RNAME : CA_LOG_TRUNCATE_DEL);
    
    pCSLog->spyLogMask = pCSLogCfg->spyLogMask;

    logOpt.pszLogFName = pCSLogCfg->pszSpyLogFName;
    logOpt.logAct = logAct;
    logOpt.dwTruncateSize = pCSLogCfg->dwSpyLogTSize;
    CA_LogUpdate(pCSLog->pSpyLog, &logOpt);

    logOpt.pszLogFName = pCSLogCfg->pszNtDumpLogFName;
    logOpt.logAct = logAct;
    logOpt.dwTruncateSize = pCSLogCfg->dwNtDumpLogTSize;
    CA_LogUpdate(pCSLog->pNtDumpLog, &logOpt);
    return CA_ERR_SUCCESS;
}

void CS_LogNtDump(const TCHAR *pszSrc, int nSrcLine, 
                  int nApiSlotId, BOOL bIsFilterProto, 
                  const char *pNtBuf, int nNtBufLen)
{
    CASpyLogMask spyLogMask;
    const TCHAR *pszSrcBase;
    CAErrno caErr;
    TCHAR szTmBuf[CA_TM_STR_MAX_BUF];
    CSLog *pCSLog = CS_LogGetPtr();

    if (NULL == pCSLog || NULL == pCSLog->pNtDumpLog)
    {
        return;
    }
    spyLogMask = pCSLog->spyLogMask;

    if (!(spyLogMask & CA_SPY_LOG_NT_DUMP) &&
        !(spyLogMask & CA_SPY_LOG_NT_ADUMP))
    {
        return;
    }

    if (!(spyLogMask & CA_SPY_LOG_NT_ADUMP) && bIsFilterProto)
    {
        return;
    }

    caErr = CA_GetCurTimeStr(szTmBuf, sizeof(szTmBuf) / sizeof(szTmBuf[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTmBuf[0] = '\0';
    }

    CA_PathGetBaseName(pszSrc, &pszSrcBase);
    CA_LogBuf(pCSLog->pNtDumpLog, pNtBuf, nNtBufLen, 
        TEXT("Time: %s, ApiSlotId: %u, Src: %s(%u) "), 
        szTmBuf, nApiSlotId, NULL == pszSrcBase ? TEXT("") : pszSrcBase, 
        nSrcLine);

}

void CS_RTLog(void *pCbCtx, CARTLog *pLog)
{
    CASpyLogMask spyLogMask;
    CAErrno caErr;
    CSLog *pCSLog = CS_LogGetPtr();
    TCHAR szTmBuf[CA_TM_STR_MAX_BUF];
    BOOL bResult;

    if (NULL == pCSLog || NULL == pCSLog->pSpyLog)
    {
        return;
    }
    spyLogMask = pCSLog->spyLogMask;

    bResult = CS_LogFalgsNeedFilter(pLog->logFlags, spyLogMask);
    if (bResult)
    {
        return;
    }

    caErr = CA_GetCurTimeStr(szTmBuf, sizeof(szTmBuf) / sizeof(szTmBuf[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTmBuf[0] = '\0';
    }

    CA_LogLine(pCSLog->pSpyLog, TEXT("%s %s %s(%u): %s\n"), 
        szTmBuf, CA_RTLogFlagsDesc(pLog->logFlags), 
        pLog->pszSrcBase, pLog->nSrcLine, pLog->pszLog);
}
