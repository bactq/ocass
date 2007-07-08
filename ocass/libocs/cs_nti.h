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

#ifndef _CS_NETI_H_
#define _CS_NETI_H_ 1

#include "ca_types.h"

#define CS_NETWRK_API_COUNT        3
#define CS_NETWRK_API_SND_VER1     0
#define CS_NETWRK_API_SND          1
#define CS_NETWRK_API_WSARCV       2

typedef struct CS_NeTWRK_API_SLOT
{
    BOOL bInUsed;
    BOOL bHooked;
    TCHAR *pszModName;
    TCHAR *pszAPIName;
    PROC pOldProc;
    PROC pNewProc;
} CSNetWrkApiSlot;

typedef struct _CS_NeTWRK_API_LIST
{
    INT nSlotId;
    TCHAR *pszModName;
    TCHAR *pszAPIName;
    PROC pNewProc;
} CSNetWrkApiList;

CSNetWrkApiSlot* CS_NtWrkGetSlot(int nIndex);

CAErrno CS_NtWrkApiAttach(void);

void CS_OnNtWrkIO(SOCKET netFd, const char *pBuf, 
                  int nBufLen, int nApiSlotId);

#endif /* !defined(_CS_NETI_H_) */
