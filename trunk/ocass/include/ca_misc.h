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

CA_DECLARE(void) CA_PathGetBaseName(const TCHAR *pszFullName, 
                                    const TCHAR **pszBaseName);

CA_DECLARE(CAErrno) CA_GetProcFirstThread(DWORD dwProcId, 
                                          DWORD *pdwFirstThId);

CA_DECLARE(CAErrno) CA_W32ResNameFromFilename(const TCHAR *pszFName, 
                        BOOL bIsGlobal, TCHAR *pszNameBuf, DWORD dwBufCnt);


#endif /* !defined(_CA_MISC_H_) */
