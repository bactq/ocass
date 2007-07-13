/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 *
 */

#ifndef _CA_OFC_H_
#define _CA_OFC_H_ 1

#include "ca_types.h"

#define CA_OFC_EXE_BASE_NAME    (TEXT("communicator.exe"))

CA_DECLARE(CAErrno) CA_OFCGetFNameFromReg(TCHAR *pszFNameBuf, 
                                          DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_OFCGetRunProcId(DWORD *pdwProcId);
CA_DECLARE(BOOL)    CA_OFCProcIsAttached(DWORD dwProcId);
CA_DECLARE(BOOL)    CA_OFCIsCommunicatorMod(HMODULE hMod);

CA_DECLARE(CAErrno) CA_OFCGetFName(TCHAR *pszFNameBuf, DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_OFCGetFNameUntruth(TCHAR *pszFNameBuf, DWORD dwBufCnt);

#endif /* !defined(_CA_OFC_H_) */

