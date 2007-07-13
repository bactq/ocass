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

#ifndef _CS_LOG_H_
#define _CS_LOG_H_ 1

#include "liboca.h"
#include "ca_types.h"
#include "ca_cfg.h"

#define CS_LOG_MAX_NTDUMP_HDR   (1024 * 2)
#define CS_LOG_MAX              (1024 * 2)

typedef struct _CSLogCfg
{
    TCHAR *pszSpyLogFName;
    DWORD dwSpyLogTSize;

    TCHAR *pszNtDumpLogFName;
    DWORD dwNtDumpLogTSize;

    CASpyLogMask spyLogMask;
} CSLogCfg;

CAErrno CS_LogStartup(CSLogCfg *pCSLogCfg);
void    CS_LogCleanup(void);

CAErrno CS_LogUpdateCfg(CSLogCfg *pCSLogCfg);

void CS_LogNtDump(const TCHAR *pszSrc, int nSrcLine, 
                  int nApiSlotId, BOOL bIsFilterProto, 
                  const char *pNtBuf, int nNtBufLen);

BOOL CS_RTLogFilter(void *pCbCtx, CARTLogFlags logFlags);
void CS_RTLog(void *pCbCtx, CARTLog *pLog);

#endif /* !defined(_CS_LOG_H_) */
