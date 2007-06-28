/**
 *
 */

#ifndef _CA_CFG_H_
#define _CA_CFG_H_ 1

#include "ca_types.h"

#define CA_CFG_FNAME        (TEXT("ocass.cfg"))

#define CA_CFG_DEFAULT_HISTORY_PATH     (TEXT("history_path"))

typedef struct _CA_CFG_DATUM
{
    TCHAR szHistoryPath[MAX_PATH];
    TCHAR szCommunicatorFName[MAX_PATH];
} CACfgDatum;

/**
 * config file sample
 *
 * [app]
 * communicator_fname="c:\abc\abc.exe"
 * history_path="c:\abc"
 */

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszWrkPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_CfgRd(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum);

CA_DECLARE(CAErrno) CA_CfgWr(const TCHAR *pszCfgFName, 
                             CACfgDatum *pCfgDatum);

CA_DECLARE(CACfgDatum*) CA_CfgGetRT(void);
CA_DECLARE(CAErrno)     CA_CfgSetRT(CACfgDatum *pCfgDatum);
CA_DECLARE(CAErrno)     CA_CfgSetRTDefault(void);
CA_DECLARE(CAErrno)     CA_CfgSetRTFromFile(const TCHAR *pszCfgFName, 
                                            const TCHAR *pszWrkPath);

#endif /* !defined(_CA_CFG_H_) */
