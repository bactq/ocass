/**
 *
 */

#ifndef _CA_CFG_H_
#define _CA_CFG_H_ 1

#include "ca_types.h"
#include "ca_sr.h"

#define CA_CFG_FNAME        (TEXT("ocass.cfg"))

#define CA_CFG_DEFAULT_HISTORY_PATH     (TEXT("history_path"))
#define CA_CFG_DEFAULT_SPY_LOG          (TEXT("ocass_s.log"))
#define CA_CFG_DEFAULT_SPY_NT_DUMP      (TEXT("ocass_ntd.log"))

typedef struct _CA_CFG_DATUM
{
    TCHAR szCommunicatorFName[MAX_PATH];

    TCHAR szHistoryPath[MAX_PATH];
    TCHAR szSpyLog[MAX_PATH];
    TCHAR szSpyNtDump[MAX_PATH];
    CASpyLogMask spyLogMask;
} CACfgDatum;

/**
 * ocass.cfg file sample
 *
 * [app]
 * communicator_fname="c:\abc\abc.exe"
 * [spy]
 * history_path="c:\abc"
 * spy_log="c:\abc\abc.log"
 * spy_ntdump="c:\abc\abc.data"
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
