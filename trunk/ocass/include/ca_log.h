/**
 *
 */

#ifndef _CA_LOG_H_
#define _CA_LOG_H_ 1

#include "ca_types.h"

typedef struct _CALog CALog;

typedef enum
{
    CA_LOG_TRUNCATE_DEL = 0, 
    CA_LOG_TRUNCATE_RNAME,
} CALogTAct;

typedef struct _CALogOpt
{
    TCHAR *pszLogFName;
    CALogTAct logAct;
    DWORD dwTruncateSize;
} CALogOpt;

#define CA_LOG_TRUNCATE_MIN_SIZE        (1024 * 1024 * 2)
#define CA_LOG_LINE_MAX                 (1024 * 4)

CA_DECLARE(CAErrno) CA_LogOpen(CALogOpt *pOpt, CALog **pLog);
CA_DECLARE(void)    CA_LogClose(CALog *pLog);

CA_DECLARE(CAErrno) CA_LogUpdate(CALog *pLog, CALogOpt *pOpt);
CA_DECLARE(CAErrno) CA_LogTruncate(CALog *pLog, BOOL bDelOld);

CA_DECLARE(void) CA_LogLine(CALog *pLog, const TCHAR *pszFmt, ...);

CA_DECLARE(void) CA_LogBuf(CALog *pLog, const char *pBuf, DWORD dwBufCnt, 
                           const TCHAR *pszHdrFmt, ...);


#endif /* !defined(_CA_LOG_H_) */
