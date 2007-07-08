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

#ifndef _CA_MM_H_
#define _CA_MM_H_ 1

#include "ca_types.h"

CA_DECLARE(void*)   CA_MAlloc(UINT nSize);
CA_DECLARE(void)    CA_MFree(void *pMem);

/**
 * @{
 */

typedef struct _CA_SHMM_ CAShMM;

CA_DECLARE(CAErrno) CA_ShMMCreate(UINT nReqSize, const TCHAR *pszFName, 
                                 CAShMM **pShMM);

CA_DECLARE(void)    CA_ShMMDestroy(CAShMM *pShMM, BOOL bUnlinkMapFile);

CA_DECLARE(CAErrno) CA_ShMMAttach(const TCHAR *pszFileName, CAShMM **pShm);
CA_DECLARE(void)    CA_ShMMDettach(CAShMM *pShm);

CA_DECLARE(void *) CA_ShMMBaseAddrGet(CAShMM *pShm);
CA_DECLARE(UINT) CA_ShMMSizeGet(CAShMM *pShm);

/** @} */

#endif /* !defined(_CA_MM_H_) */
