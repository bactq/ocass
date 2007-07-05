/**
 *
 */

#ifndef _CS_LOG_H_
#define _CS_LOG_H_ 1

#include "liboca.h"
#include "ca_types.h"
#include "ca_cfg.h"

#define CS_LOG_MAX_NTDUMP_HDR   (1024 * 2)
#define CS_LOG_MAX              (1024 * 2)

CAErrno CS_LogStartup(TCHAR *pszSpyLog, TCHAR *pszSpyNtDump, 
                      CASpyLogMask spyLogMask);

void    CS_LogCleanup(void);

CAErrno CS_LogSetLogMask(CASpyLogMask spyLogMask);
CAErrno CS_LogSetLogFName(TCHAR *pszSpyLog);
CAErrno CS_LogSetLogNtDumpFName(TCHAR *pszNtDump);


void CS_LogNtDump(const TCHAR *pszSrc, int nSrcLine, 
                  int nApiSlotId, BOOL bIsFilterProto, 
                  const char *pNtBuf, int nNtBufLen);

typedef int CSLogFlags;
#define CS_LOG_ERR      (1<< 0)
#define CS_LOG_DBG      (1<< 1)
#define CS_LOG_INFO     (1<< 2)
#define CS_LOG_WARN     (1<< 3)

void CS_Log(const TCHAR *pszSrc, int nSrcLine, CSLogFlags logFlags, 
            const TCHAR *pszFmt, ...);

void CS_RTLog(void *pCbCtx, CARTLog *pLog);

#if defined(_DEBUG)
#define CS_DBG_TRACE         CS_Log
#else
#define CS_DBG_TRACE
#endif /* defined(_DEBUG) */

#endif /* !defined(_CS_LOG_H_) */
