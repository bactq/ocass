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

#include "libocc.h"
#include "liboca.h"
#include "ca_getopt.h"
#include "ca_misc.h"
#include "ca_cfg.h"
#include "ocac_proc.h"
#include "ocac_misc.h"

CAErrno CAC_PDataInit(OCACProc *pProc)
{
    CAErrno caErr;
    
    caErr = CA_GetModPath(NULL, pProc->szProcWrkPath, 
        sizeof(pProc->szProcWrkPath) / sizeof(pProc->szProcWrkPath[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_CfgGetFName(pProc->szProcWrkPath, pProc->szProcCfgFName, 
        sizeof(pProc->szProcCfgFName) / sizeof(pProc->szProcCfgFName[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pProc->procType = OCAC_PROC_TYPE_WORKER;
    return CA_ERR_SUCCESS;  
}

CAErrno CAC_PStartup(int nArgc, char **pArgv, OCACProc *pProc)
{
    CAGetoptDatum datumGetOpt = {0};
    CAErrno caErr;
    char *pszOptArg;
    char cOpt;
    
    caErr = CAC_PDataInit(pProc);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    caErr = CA_Startup();
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    CA_RTSetLog(NULL, OCAC_LogShow);
    CA_InitGetOpt(nArgc, pArgv, &datumGetOpt);    
    for (;;)
    {
        caErr = CA_GetOpt(&datumGetOpt, "hH?Kk", &cOpt, &pszOptArg);
        if (CA_ERR_SUCCESS != caErr)
        {
            break;
        }

        switch (cOpt)
        {
        case '?':
        case 'h':
        case 'H':
            pProc->procType = OCAC_PROC_TYPE_USAGE;
            break;

        case 'K':
        case 'k':
            pProc->procType = OCAC_PROC_TYPE_KILLER;
            break;

        default:
            break;
        }
    }
    if (CA_ERR_EOF != caErr)
    {
        pProc->procType = OCAC_PROC_TYPE_USAGE;
    }

    return CA_ERR_SUCCESS;  
}

CAErrno CAC_PCleanup(OCACProc *pProc)
{
    CA_Cleanup();
    return CA_ERR_SUCCESS;
}
