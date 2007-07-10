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

#ifndef _CA_CFG_H_
#define _CA_CFG_H_ 1

#include "ca_types.h"
#include "ca_sr.h"

#define CA_CFG_FNAME        (TEXT("ocass.cfg"))

#define CA_CFG_DEFAULT_TEMPLATE_PATH    (TEXT("template"))
#define CA_CFG_DEFAULT_HISTORY_PATH     (TEXT("history"))
#define CA_CFG_DEFAULT_SPY_LOG          (TEXT("log\\ocass_s.log"))
#define CA_CFG_DEFAULT_SPY_NT_DUMP      (TEXT("log\\ocass_ntd.log"))
#define CA_CFG_DEFAULT_SHELL_LOG        (TEXT("log\\ocass.log"))

#define CA_CFG_DEFAULT_SPY_LOG_TSIZE_B        (1024 * 1024 * 2)
#define CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_B    (1024 * 1024 * 5)
#define CA_CFG_DEFAULT_SHELL_TSIZE_B          (1024 * 1024 * 2)
#define CA_CFG_DEFAULT_SPY_LOG_TSIZE_M        (2)
#define CA_CFG_DEFAULT_SPY_NT_DUMP_TSIZE_M    (5)
#define CA_CFG_DEFAULT_SHELL_TSIZE_M          (2)

typedef struct _CA_CFG_DATUM
{
    TCHAR szCommunicatorFName[MAX_PATH];

    TCHAR szHistoryPath[MAX_PATH];
    TCHAR szTemplatePath[MAX_PATH];

    TCHAR szSpyLog[MAX_PATH];
    DWORD dwSpyLogTSize;

    TCHAR szSpyNtDump[MAX_PATH];
    DWORD dwSpyNtDumpTSize;

    TCHAR szShellLog[MAX_PATH];
    DWORD dwShellTSize;

    CAShellLogMask shellLogMask;
    CASpyLogMask spyLogMask;
} CACfgDatum;

/**
 * ocass.cfg file sample
 *
 * [app]
 * communicator_fname="c:\abc\abc.exe"
 * template_path="c:\abc"
 * log_fname="abc"
 * log_ts=5
 * log_mask=0
 * [spy]
 * history_path="c:\abc"
 * spy_log="c:\abc\abc.log"
 * spy_log_ts=5
 * spy_ntdump="c:\abc\abc.data"
 * spy_ntdump_ts=5
 * spy_logmask=0
 */

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszWrkPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt);

CA_DECLARE(CAErrno) CA_CfgRd(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum);

CA_DECLARE(CAErrno) CA_CfgWr(const TCHAR *pszCfgFName, 
                             CACfgDatum *pCfgDatum);

CA_DECLARE(CAErrno)     CA_CfgDupRT(CACfgDatum *pResult);
CA_DECLARE(CACfgDatum*) CA_CfgGetRT(void);
CA_DECLARE(CAErrno)     CA_CfgSetRT(CACfgDatum *pCfgDatum);
CA_DECLARE(CAErrno)     CA_CfgSetRTDefault(void);
CA_DECLARE(CAErrno)     CA_CfgSetRTFromFile(const TCHAR *pszCfgFName, 
                                            const TCHAR *pszWrkPath);

#endif /* !defined(_CA_CFG_H_) */
