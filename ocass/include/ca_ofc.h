/**
 *
 */

#ifndef _CA_OFC_H_
#define _CA_OFC_H_ 1

#include "ca_types.h"

CA_DECLARE(CAErrno) CA_OFCGetFNameFromReg(TCHAR *pszFNameBuf, 
                                          DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_OFCGetRunProcId(DWORD *pdwProcId);

CA_DECLARE(BOOL) CA_OFCProcIsAttached(DWORD dwProcId);

#endif /* !defined(_CA_OFC_H_) */

