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

#include "liboca.h"
#include "ca_getopt.h"
#include "ca_misc.h"
#include "ca_str.h"
#include "ca_cfg.h"
#include "ca_evts.h"
#include "ocaw_main.h"
#include "ocaw_proc.h"
#include "ocaw_panic.h"
#include "ocaw_wrk.h"
#include "ocaw_log.h"

const TCHAR *OCAS_USAGE_TXT = TEXT("Useage");

int OCAS_PUseage(OCAWProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_USEAGE;

    MessageBox(NULL, OCAS_USAGE_TXT, TEXT("ocass - panic"), 
        MB_ICONINFORMATION|MB_OK);
    ExitProcess(nProcExit);
    return nProcExit;
}

int OCAS_PWakeUp(OCAWProc *pProc)
{
    HANDLE hEvt;
    int nProcExit = CA_PROC_EXIT_OK;

    hEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, OCASS_EVT_NAME_SHELL_WRK);
    if (NULL == hEvt)
    {
        return CA_PROC_EXIT_INIT_FAILED;
    }

    SetEvent(hEvt);
    CloseHandle(hEvt);
    return nProcExit;
}

CAErrno CAS_PStartup(int nArgc, char **pArgv, 
                     OCAWProc *pProc)
{
    CAGetoptDatum datumGetOpt;
    CAErrno caErr;
    HANDLE hShEvt;
    char *pzOptArg;
    char cOptCh;
    int nResult;

    caErr = CA_Startup();
    if (CA_ERR_SUCCESS != caErr)
    {
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Startup failed. Can't load run time library or config."
                 "Last Error code (%u). "), caErr);
        return caErr;
    }

    memset(pProc, 0, sizeof(OCAWProc));
    pProc->argc = nArgc;
    pProc->argv = pArgv;
    pProc->dwShellProcId = GetCurrentProcessId();
    pProc->shellProcType = OCASP_TYPE_WRK;
    pProc->bIsBackground = TRUE;
    pProc->bIsSafeMod = FALSE;
    InitializeCriticalSection(&pProc->shellCS);
    caErr = CA_GetModPath(NULL, pProc->szWrkPath, 
        sizeof(pProc->szWrkPath) / sizeof(pProc->szWrkPath[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
            TEXT("Startup failed. Can't load run time library or config."
                 "Last Error code (%u). "), caErr);
        return caErr;
    }

    CA_InitGetOpt(nArgc, pArgv, &datumGetOpt);
    for (;;)
    {
        caErr = CA_GetOpt(&datumGetOpt, "?hHf:FS", &cOptCh, &pzOptArg);
        if (CA_ERR_SUCCESS != caErr)
        {
            break;
        }

        switch (cOptCh)
        {
        case '?':
        case 'h':
        case 'H':
            pProc->shellProcType = OCASP_TYPE_USEAGE;
            break;

        case 'S':
            pProc->bIsSafeMod = TRUE;
            break;

        case 'F':
            pProc->bIsBackground = FALSE;
            break;

        case 'f':
            nResult = CA_SNPrintf(pProc->szCfgFName, 
                sizeof(pProc->szCfgFName) / sizeof(pProc->szCfgFName[0]),
                TEXT("%s"), pzOptArg);
            if (0 >= nResult)
            {
                pProc->szCfgFName[0] = '\0';
            }
            break;
        }

    }
    if (CA_ERR_EOF != caErr)
    {
        pProc->shellProcType = OCASP_TYPE_USEAGE;
    }
    if ('\0' == pProc->szCfgFName[0])
    {
        caErr = CA_CfgGetFName(pProc->szWrkPath, pProc->szCfgFName, 
            sizeof(pProc->szCfgFName) / sizeof(pProc->szCfgFName[0]));
        if (CA_ERR_SUCCESS != caErr)
        {
            CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
                TEXT("Startup failed. Can't load config."
                     "Last Error code (%u). "), caErr);
            return caErr;
        }
    }

    if (OCASP_TYPE_USEAGE != pProc->shellProcType)
    {
        hShEvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, OCASS_EVT_NAME_SHELL_WRK);
        if (NULL != hShEvt)
        {
            pProc->shellProcType = OCASP_TYPE_WAKEUP;
            CloseHandle(hShEvt);
        }
    }

    if (OCASP_TYPE_WRK == pProc->shellProcType)
    {
        caErr = CA_CfgSetRTFromFile(pProc->szCfgFName, pProc->szWrkPath);
        if (CA_ERR_SUCCESS != caErr)
        {
            CAS_Panic(CA_SRC_MARK, CA_PROC_EXIT_INIT_FAILED, 
                TEXT("Startup failed. Read config from (%s) failed."
                     "Last Error code (%u). "), pProc->szCfgFName, caErr);
            return caErr;
        }

        CAS_LogStartup();
    }

    return CA_ERR_SUCCESS;
}

void CAS_PCleanup(OCAWProc *pProc)
{
    DeleteCriticalSection(&pProc->shellCS);
    CAS_LogCleanup();
    CA_Cleanup();
}

int CAS_PRun(OCAWProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    switch (pProc->shellProcType)
    {
    case OCASP_TYPE_WRK:
        return OCAS_PWrk(pProc);

    case OCASP_TYPE_WAKEUP:
        return OCAS_PWakeUp(pProc);

    default:
    case OCASP_TYPE_USEAGE:
        return OCAS_PUseage(pProc);
    }
}

