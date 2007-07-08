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

#include "ca_log.h"
#include "ca_inner.h"
#include "ca_misc.h"
#include "ca_str.h"

struct _CALog
{
    CRITICAL_SECTION logCS;

    TCHAR szLogFName[MAX_PATH];
    CALogTAct logAct;
    DWORD dwTruncateSize;

    FILE *pLogFD;
    CAFSize logFSize;
};

static BOOL CA_LogIsNeedTruncate(CALog *pLog)
{
    if (NULL == pLog || NULL == pLog->pLogFD)
    {
        return FALSE;
    }

    if (pLog->logFSize.dwFSize >= pLog->dwTruncateSize)
    {
        return TRUE;
    }

    return FALSE;
}

CA_DECLARE(CAErrno) CA_LogOpen(CALogOpt *pOpt, CALog **pLog)
{
    CRITICAL_SECTION *pLogCS = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    CALog *pNewLog = NULL;
    FILE *pLogFD = NULL;
    int nResult;

    pNewLog = (CALog *)CA_MAlloc(sizeof(CALog));
    if (NULL == pNewLog)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    memset(pNewLog, 0, sizeof(CALog));
    nResult = CA_SNPrintf(pNewLog->szLogFName, 
        sizeof(pNewLog->szLogFName) / sizeof(pNewLog->szLogFName[0]), 
        TEXT("%s"), pOpt->pszLogFName);
    if (0 >= nResult)
    {
        funcErr = CA_ERR_FNAME_TOO_LONG;
        goto EXIT;
    }

    caErr = CA_GetFSize(pNewLog->szLogFName, &pNewLog->logFSize);
    if (CA_ERR_SUCCESS != caErr)
    {
        pNewLog->logFSize.dwFSize = 0;
        pNewLog->logFSize.dwFSizeHigh = 0;
    }

    pNewLog->pLogFD = fopen(pNewLog->szLogFName, "a+");
    if (NULL == pNewLog->pLogFD)
    {
        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }
    pLogFD = pNewLog->pLogFD;

    InitializeCriticalSection(&pNewLog->logCS);
    pLogCS = &pNewLog->logCS;

    pNewLog->logAct = pOpt->logAct;
    pNewLog->dwTruncateSize = 
        (CA_LOG_TRUNCATE_MIN_SIZE >= pOpt->dwTruncateSize ? 
            CA_LOG_TRUNCATE_MIN_SIZE : pOpt->dwTruncateSize);
EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pLogCS)
        {
            DeleteCriticalSection(pLogCS);
        }

        if (NULL != pLogFD)
        {
            fclose(pLogFD);
        }

        CA_MFree(pNewLog);
    }
    else
    {
        *pLog = pNewLog;
    }
    return funcErr;
}

CA_DECLARE(void) CA_LogClose(CALog *pLog)
{
    if (NULL == pLog)
    {
        return;
    }

    if (NULL != pLog->pLogFD)
    {
        fclose(pLog->pLogFD);
    }

    DeleteCriticalSection(&pLog->logCS);
    CA_MFree(pLog);
}

CA_DECLARE(void) CA_LogLine(CALog *pLog, const TCHAR *pszFmt, ...)
{
    va_list pArgList;
    TCHAR szLogLine[CA_LOG_LINE_MAX];
    DWORD dwWrCnt;
    BOOL bNeedTruncate;
    int nResult;

    if (NULL == pLog || NULL == pLog->pLogFD)
    {
        return;
    }

    va_start(pArgList, pszFmt);
    nResult = CA_VSNPrintf(szLogLine, 
        sizeof(szLogLine) / sizeof(szLogLine[0]), pszFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        return;
    }

    bNeedTruncate = FALSE;
    dwWrCnt = 0;
    EnterCriticalSection(&pLog->logCS);
    if (NULL != pLog->pLogFD)
    {
        dwWrCnt = fprintf(pLog->pLogFD, "%s\n", szLogLine);

        fflush(pLog->pLogFD);
        CA_AddFSize(&pLog->logFSize, dwWrCnt);
        bNeedTruncate = CA_LogIsNeedTruncate(pLog);
    }
    LeaveCriticalSection(&pLog->logCS);
    if (bNeedTruncate)
    {
        CA_LogTruncate(pLog, 
            CA_LOG_TRUNCATE_DEL == pLog->logAct ? TRUE : FALSE);
    }
}

CA_DECLARE(void) CA_LogBuf(CALog *pLog, const char *pBuf, DWORD dwBufCnt, 
                           const TCHAR *pszHdrFmt, ...)
{
    va_list pArgList;
    TCHAR szLogLine[CA_LOG_LINE_MAX];
    DWORD dwWrCnt;
    DWORD dwBufOffset;
    BOOL bNeedTruncate;
    char cPos;
    int nResult;

    if (NULL == pLog || NULL == pLog->pLogFD)
    {
        return;
    }

    va_start(pArgList, pszHdrFmt);
    nResult = CA_VSNPrintf(szLogLine, 
        sizeof(szLogLine) / sizeof(szLogLine[0]), pszHdrFmt, pArgList);
    va_end(pArgList);
    if (0 >= nResult)
    {
        return;
    }

    bNeedTruncate = FALSE;
    dwWrCnt = 0;
    EnterCriticalSection(&pLog->logCS);
    if (NULL != pLog->pLogFD)
    {
        nResult = fprintf(pLog->pLogFD, "\n^^^^^^^^^\n");
        if (0 < nResult)
        {
            dwWrCnt += nResult;
        }

        nResult = fprintf(pLog->pLogFD, "Hdr: %s\n\n", szLogLine);
        if (0 < nResult)
        {
            dwWrCnt += nResult;
        }

        for (dwBufOffset = 0; dwBufOffset < dwBufCnt; dwBufOffset++)
        {
            cPos = pBuf[dwBufOffset];
            nResult = fprintf(pLog->pLogFD, "%c", cPos);
            if (0 < nResult)
            {
                dwWrCnt += nResult;
            }
        }

        nResult = fprintf(pLog->pLogFD, "\n\nvvvvvvvvv\n");
        if (0 < nResult)
        {
            dwWrCnt += nResult;
        }

        fflush(pLog->pLogFD);
        CA_AddFSize(&pLog->logFSize, dwWrCnt);
        bNeedTruncate = CA_LogIsNeedTruncate(pLog);
    }
    LeaveCriticalSection(&pLog->logCS);
    if (bNeedTruncate)
    {
        CA_LogTruncate(pLog, 
            CA_LOG_TRUNCATE_DEL == pLog->logAct ? TRUE : FALSE);
    }
}

CA_DECLARE(CAErrno) CA_LogUpdate(CALog *pLog, CALogOpt *pOpt)
{
    TCHAR szLogFName[MAX_PATH];
    int nResult;

    nResult = CA_SNPrintf(szLogFName, 
        sizeof(szLogFName) / sizeof(szLogFName[0]), 
        TEXT("%s"), pOpt->pszLogFName);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    EnterCriticalSection(&pLog->logCS);
    pLog->logAct = pOpt->logAct;
    pLog->dwTruncateSize = 
        (CA_LOG_TRUNCATE_MIN_SIZE >= pOpt->dwTruncateSize ? 
            CA_LOG_TRUNCATE_MIN_SIZE : pOpt->dwTruncateSize);

    nResult = lstrcmpi(szLogFName, pLog->szLogFName);
    if (0 != nResult)
    {
        lstrcpy(pLog->szLogFName, szLogFName);
        pLog->pLogFD = fopen(pLog->szLogFName, "a+");
        CA_GetFSize(pLog->szLogFName, &pLog->logFSize);
    }
    LeaveCriticalSection(&pLog->logCS);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_LogTruncate(CALog *pLog, BOOL bDelOld)
{
    WIN32_FIND_DATA findData;
    CAErrno funcErr = CA_ERR_SUCCESS;
    HANDLE hFind;
    TCHAR szFName[MAX_PATH];
    BOOL bFind;
    int nRName;
    int nResult;

    if (NULL == pLog || NULL == pLog->pLogFD)
    {
        return CA_ERR_SUCCESS;
    }

    EnterCriticalSection(&pLog->logCS);
    fclose(pLog->pLogFD);
    pLog->pLogFD = NULL;

    if (bDelOld)
    {
        DeleteFile(pLog->szLogFName);
        pLog->pLogFD = fopen(pLog->szLogFName, "a+");
        CA_SetFSize(&pLog->logFSize, 0, 0);
        goto EXIT;
    }

    for (nRName = 0, bFind = FALSE;;nRName++)
    {
        nResult = CA_SNPrintf(szFName, sizeof(szFName) / sizeof(szFName[0]), 
            TEXT("%s_arc%u"), pLog->szLogFName, nRName);
        if (0 >= nResult)
        {
            funcErr = CA_ERR_FNAME_TOO_LONG;
            goto EXIT;
        }

        hFind = FindFirstFile(szFName, &findData);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            bFind = TRUE;
            break;
        }
        FindClose(hFind);
    }
    if (!bFind)
    {
        funcErr = CA_ERR_FNAME_BAD;
        goto EXIT;
    }

    MoveFile(pLog->szLogFName, szFName);
    DeleteFile(pLog->szLogFName);
    pLog->pLogFD = fopen(pLog->szLogFName, "a+");
    CA_SetFSize(&pLog->logFSize, 0, 0);
EXIT:
    LeaveCriticalSection(&pLog->logCS);
    return funcErr;
}
