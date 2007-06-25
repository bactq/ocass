/**
 *
 */

#ifndef _CA_STR_H_
#define _CA_STR_H_ 1

#include "ca_types.h"

CA_DECLARE(CHAR *) CA_StrTok(CHAR *pszStr, const CHAR *pszSep, 
                             CHAR **pszLast);

CA_DECLARE_NOSTD(int) CA_SNPrintf(TCHAR *pszBuf, const int nBufCnt, 
                                  const TCHAR *pszFmt, ...);

#endif /* !defined(_CA_STR_H_) */
