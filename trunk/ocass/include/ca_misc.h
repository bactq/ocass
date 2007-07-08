/*
 * Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 * This file is part of ocass
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#define CA_TM_STR_MAX_BUF   (128)

CA_DECLARE(CAErrno) CA_GetCurTimeStr(TCHAR *pszTmBuf, DWORD dwBufCnt);
CA_DECLARE(CAErrno) CA_GetTimeStr(time_t tmVal, TCHAR *pszTmBuf, 
                                  DWORD dwBufCnt);

typedef struct _CAFSize 
{
    DWORD dwFSize;
    DWORD dwFSizeHigh;
} CAFSize;

CA_DECLARE(void) CA_SetFSize(CAFSize *pFSize, 
                             DWORD dwFSize, DWORD dwFSizeHigh);

CA_DECLARE(void) CA_AddFSize(CAFSize *pFSize, 
                             DWORD dwIncreaseFSize);

CA_DECLARE(CAErrno) CA_GetFSize(const TCHAR *pszFName, 
                                CAFSize *pFSize);

CA_DECLARE(CAErrno) CA_MkSubDirWithBName(const TCHAR *pszDir, 
                                         TCHAR *pszRealNameBuf, 
                                         DWORD dwNameBufCnt);

CA_DECLARE(CAErrno) CA_ConvertFNameStr(const TCHAR *pszSrc, 
                        TCHAR *pszFNameBuf, DWORD dwFNameBufCnt);

#endif /* !defined(_CA_MISC_H_) */
