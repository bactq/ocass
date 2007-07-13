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

#include <windows.h>
#include "liboca.h"
#include "ca_str.h"
#include "ca_misc.h"
#include "ocaw_panic.h"

void CAS_Panic(const TCHAR *pszSrc, UINT nSrcLine, 
               int nProcExit, const TCHAR *pszReasonFmt, ...)
{
    const TCHAR *pszBaseName;
    va_list pArgList;
    TCHAR szOutMsg[1024 * 4];
    TCHAR szMsg[1024 * 3];
    int nResult;

    va_start(pArgList, pszReasonFmt);
    nResult = CA_VSNPrintf(szMsg, sizeof(szMsg) / sizeof(szMsg[0]), 
        pszReasonFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        goto EXIT;
    }

    CA_PathGetBaseName(pszSrc, &pszBaseName);
    nResult = CA_SNPrintf(szOutMsg, sizeof(szOutMsg) / sizeof(szOutMsg[0]), 
        TEXT("Panic:        %s (%u)\r\n"
             "Exit code:    %u\r\n"
             "Reason:       \r\n\r\n"
             "%s\r\n"),
             pszBaseName, nSrcLine, nProcExit, szMsg);
    if (0 >= nResult)
    {
        goto EXIT;
    }

    MessageBox(NULL, szOutMsg, TEXT("ocass - panic"), 
        MB_ICONWARNING|MB_OK);
EXIT:
    ExitProcess(nProcExit);
}

void CAS_PanicNoMsg(const TCHAR *pszSrc, UINT nSrcLine, int nProcExit)
{
    CA_RTLog(pszSrc, nSrcLine, CA_RTLOG_WARN, 
        TEXT("ocass shell panic no reason, exit code %u"), nProcExit);
    ExitProcess(nProcExit);
}
