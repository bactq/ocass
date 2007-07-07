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

typedef struct _CSLogCfg
{
    TCHAR *pszSpyLogFName;
    DWORD dwSpyLogTSize;

    TCHAR *pszNtDumpLogFName;
    DWORD dwNtDumpLogTSize;

    CASpyLogMask spyLogMask;
} CSLogCfg;

CAErrno CS_LogStartup(CSLogCfg *pCSLogCfg);
void    CS_LogCleanup(void);

CAErrno CS_LogUpdateCfg(CSLogCfg *pCSLogCfg);

void CS_LogNtDump(const TCHAR *pszSrc, int nSrcLine, 
                  int nApiSlotId, BOOL bIsFilterProto, 
                  const char *pNtBuf, int nNtBufLen);

void CS_RTLog(void *pCbCtx, CARTLog *pLog);

#endif /* !defined(_CS_LOG_H_) */
