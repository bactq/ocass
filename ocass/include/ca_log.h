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

#ifndef _CA_LOG_H_
#define _CA_LOG_H_ 1

#include "ca_types.h"

typedef struct _CALog CALog;

typedef enum
{
    CA_LOG_TRUNCATE_DEL = 0, 
    CA_LOG_TRUNCATE_RNAME,
} CALogTAct;

typedef struct _CALogOpt
{
    TCHAR *pszLogFName;
    CALogTAct logAct;
    DWORD dwTruncateSize;
} CALogOpt;

#define CA_LOG_TRUNCATE_MIN_SIZE        (1024 * 1024 * 2)
#define CA_LOG_LINE_MAX                 (1024 * 4)

CA_DECLARE(CAErrno) CA_LogOpen(CALogOpt *pOpt, CALog **pLog);
CA_DECLARE(void)    CA_LogClose(CALog *pLog);

CA_DECLARE(CAErrno) CA_LogUpdate(CALog *pLog, CALogOpt *pOpt);
CA_DECLARE(CAErrno) CA_LogTruncate(CALog *pLog, BOOL bDelOld);

CA_DECLARE(void) CA_LogLine(CALog *pLog, const TCHAR *pszFmt, ...);

CA_DECLARE(void) CA_LogBuf(CALog *pLog, const char *pBuf, DWORD dwBufCnt, 
                           const TCHAR *pszHdrFmt, ...);


#endif /* !defined(_CA_LOG_H_) */
