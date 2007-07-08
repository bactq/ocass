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

#ifndef _CA_CHR_H_
#define _CA_CHR_H_ 1

#include "ca_types.h"

typedef struct _CA_CHART_HISTORY_REC CACHRec;

#define CA_CHR_DEFAULT_CACHE_SEC        (60 * 3)

CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR, DWORD nMaxCacheSec);

CA_DECLARE(CAErrno) CA_CHRecClose(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRecCloseTimeOutSlot(CACHRec *pCHR);
CA_DECLARE(CAErrno) CA_CHRecCloseAllSlot(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRCpStyleFile(CACHRec *pCHR, 
                                      const TCHAR *pszDestPath);

typedef struct _CA_CHR_ITEM
{
    time_t tmAppend;
    char *pszMaster;
    char *pszGuest;

    char *pszCallId;
    DWORD dwCSeq;

    char *pszFrom;
    char *pszTo;

    char *pszMsgData;
    WCHAR *pwszMsgData;
} CACHRItem;

#define CA_CHR_MAX_RETRY        (10)

CA_DECLARE(CAErrno) CA_CHRecAppend(CACHRec *pCHR, DWORD dwRetryCnt, 
                                   CACHRItem *pCHRItem);

#endif /* !defined(_CA_CHR_H_) */
