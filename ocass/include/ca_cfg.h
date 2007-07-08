/**
 *
 */

#ifndef _CA_CFG_H_
#define _CA_CFG_H_ 1

#include "ca_types.h"
#include "ca_sr.h"

#define CA_CFG_FNAME        (TEXT("ocass.cfg"))

#define CA_CFG_DEFAULT_TEMPLATE_PATH    (TEXT("template"))
#define CA_CFG_DEFAULT_HISTORY_PATH     (TEXT("history"))
#define CA_CFG_DEFAULT_SPY_LOG          (TEXT("ocass_s.log"))
#define CA_CFG_DEFAULT_SPY_NT_DUMP      (TEXT("ocass_ntd.log"))

#define CA_CFG_DEFAULT_SPY_LOG_TSIZE_B        (1024 * 1024 * 3)
#define CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_B    (1024 * 1024 * 5)
#define CA_CFG_DEFAULT_SPY_LOG_TSIZE_M        (3)
#define CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_M    (5)

typedef struct _CA_CFG_DATUM
{
    TCHAR szCommunicatorFName[MAX_PATH];

    TCHAR szHistoryPath[MAX_PATH];
    TCHAR szTemplatePath[MAX_PATH];

    TCHAR szSpyLog[MAX_PATH];
    DWORD dwSpyLogTSize;

    TCHAR szSpyNtDump[MAX_PATH];
    DWORD dwSpyNtDumpTSize;

    CASpyLogMask spyLogMask;
} CACfgDatum;

/**
 * ocass.cfg file sample
 *
 * [app]
 * communicator_fname="c:\abc\abc.exe"
 * template_path="c:\abc"
 * [spy]
 * history_path="c:\abc"
 * spy_log="c:\abc\abc.log"
 * spy_log_ts=5
 * spy_ntdump="c:\abc\abc.data"
 * spy_ntdump_ts=5
 * spy_logmask=0
 */

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszWrkPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_CfgRd(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum);

CA_DECLARE(CAErrno) CA_CfgWr(const TCHAR *pszCfgFName, 
                             CACfgDatum *pCfgDatum);

CA_DECLARE(CAErrno)     CA_CfgDupRT(CACfgDatum *pResult);
CA_DECLARE(CACfgDatum*) CA_CfgGetRT(void);
CA_DECLARE(CAErrno)     CA_CfgSetRT(CACfgDatum *pCfgDatum);
CA_DECLARE(CAErrno)     CA_CfgSetRTDefault(void);
CA_DECLARE(CAErrno)     CA_CfgSetRTFromFile(const TCHAR *pszCfgFName, 
                                            const TCHAR *pszWrkPath);

#endif /* !defined(_CA_CFG_H_) */
