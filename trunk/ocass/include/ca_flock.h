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

#ifndef _CA_FLOCK_H_
#define _CA_FLOCK_H_ 1

#include "ca_types.h"

typedef struct _CA_FLock_
{
    HANDLE hFile;
	char szLockFName[MAX_PATH];
	OVERLAPPED overLapped;
} CAFLock;

typedef enum
{
    CA_FLOCK_OPT_RD = 1, 
    CA_FLOCK_OPT_RW, 
} CAFLockOpt;

CA_DECLARE(CAErrno) CA_FLockCreate(const char *pszFileName, CAFLock *pFLock);
CA_DECLARE(CAErrno) CA_FLockDestroy(CAFLock *pFLock, BOOL bUnlinkLockFile);

CA_DECLARE(CAErrno) CA_FLockEnter(CAFLock *pFLock, CAFLockOpt lockOpt);
CA_DECLARE(CAErrno) CA_FLockLeave(CAFLock *pFLock);


#endif /* !defined(_CA_FLOCK_H_) */
