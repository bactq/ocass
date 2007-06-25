/**
 *
 */

#ifndef _CA_MISC_H_
#define _CA_MISC_H_ 1

#include "ca_types.h"

CA_DECLARE(CAErrno) CA_GetModPath(HMODULE hMod, 
                                  TCHAR *pszFNameBuf, DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_PathJoin(const TCHAR *pszPath, 
                                const TCHAR *pszSubPath, 
                                TCHAR *pszPathBuf, DWORD dwBufCnt);


#endif /* !defined(_CA_MISC_H_) */
