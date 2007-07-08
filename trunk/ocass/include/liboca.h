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

#ifndef _LIBOCA_H_
#define _LIBOCA_H_ 1

#include "ca_types.h"
#include "ca_flock.h"

CA_DECLARE(CAErrno) CA_Startup(void);
CA_DECLARE(void)    CA_Cleanup(void);

CA_DECLARE(CAErrno) CA_RTCSEnter(void);
CA_DECLARE(CAErrno) CA_RTCSLeave(void);

#define CA_RT_PL_FNAME      (TEXT("ocass_rt.lock"))

CA_DECLARE(CAErrno) CA_RTPLockEnter(CAFLockOpt lockOpt);
CA_DECLARE(CAErrno) CA_RTPLockLeave(void);

typedef int CARTLogFlags;
#define CA_RTLOG_ERR        (1<< 0)
#define CA_RTLOG_INFO       (1<< 1)
#define CA_RTLOG_WARN       (1<< 2)
#define CA_RTLOG_DBG        (1<< 3)
#define CA_RTLOG_OSERR      (1<<10)

typedef struct _CA_RTLog
{
    const TCHAR *pszSrc;
    const TCHAR *pszSrcBase;
    int nSrcLine;

    CARTLogFlags logFlags;
    const TCHAR *pszLog;
} CARTLog;

typedef void (*CA_RTLogFunc)(void *pCbCtx, CARTLog *pLog);

CA_DECLARE(CAErrno) CA_RTSetLog(void *pCbCtx, CA_RTLogFunc pLogFunc);

CA_DECLARE(const TCHAR*) CA_RTLogFlagsDesc(CARTLogFlags logFlags);

CA_DECLARE(void) CA_RTLog(const TCHAR *pszSrc, int nSrcLine, 
                          CARTLogFlags logFlags, const TCHAR *pszFmt, ...);

#if defined(_DEBUG)
#define CA_RT_DBG_TRACE         CA_RTLog
#else
#define CA_RT_DBG_TRACE
#endif /* defined(_DEBUG) */

#endif /* !defined(_LIBOCA_H_) */
