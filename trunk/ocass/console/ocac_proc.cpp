/**
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
