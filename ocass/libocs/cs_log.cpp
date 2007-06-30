/**
 *
 */

#include "ca_str.h"
#include "cs_log.h"
#include "ca_misc.h"

typedef struct _CSLog
{
    CRITICAL_SECTION logCS;

    TCHAR szSpyLog[MAX_PATH];
    FILE *pSpyLog;
    CAFSize spyLogSize;

    TCHAR szNtDump[MAX_PATH];
    FILE *pNtDump;
    CAFSize ntDumpLogSize;

    CASpyLogMask spyLogMask;
} CSLog;

static CSLog g_csLog = {0};
static CSLog *g_pCSLog = NULL;

static CSLog* CS_LogGetPtr(void)
{
    return g_pCSLog;
}

static const TCHAR *CS_LogFalgsDesc(CSLogFlags logFlags)
{
    return TEXT("");
}

static BOOL CS_LogFalgsNeedFilter(CSLogFlags logFlags, 
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
        (!(logFlags & CS_LOG_ERR) && !(logFlags & CS_LOG_WARN) && 
         !(logFlags & CS_LOG_INFO)))
    {
        return TRUE;
    }
    else if ((CA_SPY_LOG_WARN & spyLogMask) && 
        (!(logFlags & CS_LOG_ERR) && !(logFlags & CS_LOG_WARN)))
    {
        return TRUE;
    }
    else if ((CA_SPY_LOG_ERR & spyLogMask) &&
        !(logFlags & CS_LOG_ERR))
    {
        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

static DWORD CS_LogWrBin(CSLog *pCSLog, FILE *pLogFd, 
                         const char *pBuf, int nBufLen, 
                         TCHAR *pszHdrFmt, ...)
{
    TCHAR szNtDumpLog[CS_LOG_MAX_NTDUMP_HDR];
    va_list pArgList;
    char cPos;
    int nWrCnt = 0;
    int nResult;
    int i;

    if (NULL == pLogFd)
    {
        return nWrCnt;
    }

    va_start(pArgList, pszHdrFmt);
    nResult = CA_VSNPrintf(szNtDumpLog, 
        sizeof(szNtDumpLog) / sizeof(szNtDumpLog[0]), 
        pszHdrFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        return nWrCnt;
    }

    nResult = fprintf(pLogFd, "\n^^^^^^^^^\n");
    if (0 < nResult)
    {
        nWrCnt += nResult;
    }

    nResult = fprintf(pLogFd, "Hdr: %s\n\n", szNtDumpLog);
    if (0 < nResult)
    {
        nWrCnt += nResult;
    }

    for (i = 0; i < nBufLen; i++)
    {
        cPos = pBuf[i];
        nResult = fprintf(pLogFd, "%c", cPos);
        if (0 < nResult)
        {
            nWrCnt += nResult;
        }
    }

    nResult = fprintf(pLogFd, "\n\nvvvvvvvvv\n");
    if (0 < nResult)
    {
        nWrCnt += nResult;
    }

    fflush(pLogFd);
    return nWrCnt;
}

CAErrno CS_LogStartup(TCHAR *pszSpyLog, TCHAR *pszSpyNtDump, 
                      CASpyLogMask spyLogMask)
{
    CRITICAL_SECTION *pLogCS = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    int nResult;

    g_csLog.spyLogMask = spyLogMask;
    InitializeCriticalSection(&g_csLog.logCS);

    nResult = CA_SNPrintf(g_csLog.szSpyLog,
        sizeof(g_csLog.szSpyLog) / sizeof(g_csLog.szSpyLog[0]), 
        TEXT("%s"), pszSpyLog);
    if (0 >= nResult)
    {
        g_csLog.pSpyLog = NULL;
        g_csLog.szSpyLog[0] = '\0';
        CA_SetFSize(&g_csLog.spyLogSize, 0, 0);
    }
    else
    {
        CA_GetFSize(g_csLog.szSpyLog, &g_csLog.spyLogSize);
        g_csLog.pSpyLog = fopen(g_csLog.szSpyLog, "a+");
    }

    nResult = CA_SNPrintf(g_csLog.szNtDump, 
        sizeof(g_csLog.szNtDump) / sizeof(g_csLog.szNtDump[0]), 
        TEXT("%s"), pszSpyNtDump);
    if (0 >= nResult)
    {
        g_csLog.pNtDump = NULL;
        g_csLog.szNtDump[0] = '\0';
        CA_SetFSize(&g_csLog.ntDumpLogSize, 0, 0);
    }
    else
    {
        CA_GetFSize(g_csLog.szNtDump, &g_csLog.ntDumpLogSize);
        g_csLog.pNtDump = fopen(g_csLog.szNtDump, "a+");
    }

    g_pCSLog = &g_csLog;
    return funcErr;
}

void CS_LogCleanup(void)
{
    g_pCSLog = NULL;

    /* Close all file */
}

CAErrno CS_LogSetLogMask(CASpyLogMask spyLogMask)
{
    return CA_ERR_SUCCESS;
}

CAErrno CS_LogSetLogFName(TCHAR *pszSpyLog)
{
    return CA_ERR_SUCCESS;
}

CAErrno CS_LogSetLogNtDumpFName(TCHAR *pszNtDump)
{
    return CA_ERR_SUCCESS;
}

void CS_LogNtDump(const TCHAR *pszSrc, int nSrcLine, 
                  int nApiSlotId, BOOL bIsFilterProto, 
                  const char *pNtBuf, int nNtBufLen)
{   
    CASpyLogMask spyLogMask;
    const TCHAR *pszSrcBase;
    CAErrno caErr;
    CSLog *pCSLog = CS_LogGetPtr();
    TCHAR szTmBuf[CA_TM_STR_MAX_BUF];
    DWORD dwWrCnt;
    BOOL bIsOpen = FALSE;
    
    if (NULL == pCSLog)
    {
        return;
    }

    EnterCriticalSection(&pCSLog->logCS);
    if (NULL != pCSLog->pNtDump)
    {
        bIsOpen = TRUE;
    }
    spyLogMask = pCSLog->spyLogMask;
    LeaveCriticalSection(&pCSLog->logCS);

    if (!bIsOpen || (!(spyLogMask & CA_SPY_LOG_NT_DUMP) &&
        !(spyLogMask & CA_SPY_LOG_NT_ADUMP)))
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
    EnterCriticalSection(&pCSLog->logCS);
    dwWrCnt = CS_LogWrBin(pCSLog, pCSLog->pNtDump, pNtBuf, nNtBufLen, 
        TEXT("Time: %s, ApiSlotId: %u, Src: %s(%u) "), 
        szTmBuf, nApiSlotId, NULL == pszSrcBase ? TEXT("") : pszSrcBase, 
        nSrcLine);

    CA_AddFSize(&pCSLog->ntDumpLogSize, dwWrCnt);
    LeaveCriticalSection(&pCSLog->logCS);
}

void CS_Log(const TCHAR *pszSrc, int nSrcLine, CSLogFlags logFlags, 
            const TCHAR *pszFmt, ...)
{
    CASpyLogMask spyLogMask;
    const TCHAR *pszSrcBase;
    CAErrno caErr;
    va_list pArgList;
    CSLog *pCSLog = CS_LogGetPtr();
    BOOL bIsOpen = FALSE;
    TCHAR szSLog[CS_LOG_MAX];
    TCHAR szTmBuf[CA_TM_STR_MAX_BUF];
    int nResult;

    if (NULL == pCSLog)
    {
        return;
    }

    EnterCriticalSection(&pCSLog->logCS);
    if (NULL != pCSLog->pNtDump)
    {
        bIsOpen = TRUE;
    }
    spyLogMask = pCSLog->spyLogMask;
    LeaveCriticalSection(&pCSLog->logCS);
    if (!bIsOpen)
    {
        return;
    }

    if (CS_LogFalgsNeedFilter(logFlags, spyLogMask))
    {
        return;
    }

    va_start(pArgList, pszFmt);
    nResult = CA_VSNPrintf(szSLog, sizeof(szSLog) / sizeof(szSLog[0]), 
        pszFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        return;
    }

    caErr = CA_GetCurTimeStr(szTmBuf, sizeof(szTmBuf) / sizeof(szTmBuf[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTmBuf[0] = '\0';
    }

    CA_PathGetBaseName(pszSrc, &pszSrcBase);
    EnterCriticalSection(&pCSLog->logCS);
    if (NULL != pCSLog->pSpyLog)
    {
        nResult = fprintf(pCSLog->pSpyLog, "%s %s %s(%u): %s\n", szTmBuf, 
            CS_LogFalgsDesc(logFlags),
            NULL == pszSrcBase ? "" : pszSrcBase, nSrcLine, 
            szSLog);        
        if (0 < nResult)
        {
            fflush(pCSLog->pSpyLog);
            CA_AddFSize(&pCSLog->spyLogSize, nResult);
        }
    }
    LeaveCriticalSection(&pCSLog->logCS);
}
