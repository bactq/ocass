
#ifndef _LIBOCA_H_
#define _LIBOCA_H_ 1

#include "ca_types.h"
#include "ca_flock.h"

CA_DECLARE(CAErrno) CA_Startup(void);
CA_DECLARE(void)    CA_Cleanup(void);

CA_DECLARE(CAErrno) CA_RTCSEnter(void);
CA_DECLARE(CAErrno) CA_RTCSLeave(void);

#define CA_RT_PL_FNAME      (TEXT("ocass_rt.lock"))

CA_DECLARE(CAErrno) CA_RTPLockEnter(CAFLockOpt lockOpt);
CA_DECLARE(CAErrno) CA_RTPLockLeave(void);

typedef int CARTLogFlags;
#define CA_RTLOG_ERR        (1<< 0)
#define CA_RTLOG_INFO       (1<< 1)
#define CA_RTLOG_WARN       (1<< 2)
#define CA_RTLOG_DBG        (1<< 3)

typedef struct _CA_RTLog
{
    const TCHAR *pszSrc;
    const TCHAR *pszSrcBase;
    int nSrcLine;

    CARTLogFlags logFlags;
    const TCHAR *pszLog;
} CARTLog;

typedef void (*CA_RTLogFunc)(void *pCbCtx, CARTLog *pLog);

CA_DECLARE(CAErrno) CA_RTSetLog(void *pCbCtx, CA_RTLogFunc pLogFunc);

CA_DECLARE(const TCHAR*) CA_RTLogFlagsDesc(CARTLogFlags logFlags);

CA_DECLARE(void) CA_RTLog(const TCHAR *pszSrc, int nSrcLine, 
                          CARTLogFlags logFlags, const TCHAR *pszFmt, ...);

#if defined(_DEBUG)
#define CA_RT_DBG_TRACE         CA_RTLog
#else
#define CA_RT_DBG_TRACE
#endif /* defined(_DEBUG) */

#endif /* !defined(_LIBOCA_H_) */
