/**
 *
 */

#ifndef _CS_SR_H_
#define _CS_SR_H_ 1

#include "ca_types.h"

typedef int CASpyLogMask;
#define CA_SPY_LOG_NONE         0
#define CA_SPY_LOG_ERR          (1<< 0)
#define CA_SPY_LOG_DBG          (1<< 1)
#define CA_SPY_LOG_INFO         (1<< 2)
#define CA_SPY_LOG_NT_DUMP      (1<<10)

typedef struct _CA_SD
{
    TCHAR szHistoryPath[MAX_PATH];
    TCHAR szSpyLog[MAX_PATH];
    TCHAR szSpyNtDump[MAX_PATH];
    CASpyLogMask logMask;
} CASpyDatum;

#define CA_SPY_RUN_DATUM_FNAME      (TEXT("ocass_s.datum"))
#define CA_SPY_RUN_LOCK_FNAME       (TEXT("ocass_s.lock"))

typedef struct _CA_SR CASpyRun;

CA_DECLARE(CAErrno) CA_SRCreate(CASpyRun **pSR, BOOL bAutoDupCfg);
CA_DECLARE(CAErrno) CA_SRAttach(CASpyRun **pSR);
CA_DECLARE(void)    CA_SRClose(CASpyRun *pSR);

CA_DECLARE(CAErrno)     CA_SRLock(CASpyRun *pSR, BOOL bWr);
CA_DECLARE(CAErrno)     CA_SRUnlock(CASpyRun *pSR);
CA_DECLARE(CASpyDatum*) CA_SRGetDatum(CASpyRun *pSR);
CA_DECLARE(CAErrno)     CA_SRTouch(CASpyRun *pSR);


#endif /* !defined(_CS_SR_H_) */
