/**
 *
 */

#ifndef _CA_CFG_H_
#define _CA_CFG_H_ 1

#include "ca_types.h"

#define CA_CFG_FNAME        (TEXT("ocass.cfg"))

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt);

#endif /* !defined(_CA_CFG_H_) */

