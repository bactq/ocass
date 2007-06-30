/**
 *
 */

#ifndef _CS_SR_H_
#define _CS_SR_H_ 1

#include "ca_types.h"

typedef int CASpyLogMask;
#define CA_SPY_LOG_NONE         (0)
#define CA_SPY_LOG_ERR          (1<< 0)
#define CA_SPY_LOG_DBG          (1<< 1)
#define CA_SPY_LOG_WARN         (1<< 2)
#define CA_SPY_LOG_INFO         (1<< 3)
#define CA_SPY_LOG_NT_DUMP      (1<<10)
#define CA_SPY_LOG_NT_ADUMP     (1<<11)

typedef enum
{
    CA_SPY_STATE_PREPARE = 0,
    CA_SPY_STATE_RUNNING, 
    CA_SPY_STATE_END,
    CA_SPY_STATE_FAILED,
} CASpyState;

typedef struct _CA_SD
{
    CASpyState  spyState;
    time_t      stateStartTime;
    BOOL        bStateIsDirty;

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

CA_DECLARE(void)        CA_SRDatumDup(CASpyRun *pSR, CASpyDatum *pDup);
CA_DECLARE(void)        CA_SRUpdateState(CASpyRun *pSR, CASpyState spyState);

#endif /* !defined(_CS_SR_H_) */
