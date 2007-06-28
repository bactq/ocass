/**
 *
 */

#include "liboch.h"
#include "ca_cfg.h"
#include "ocac_wrk.h"
#include "ocac_misc.h"

static int OCAC_PCCWrk(OCACProc *pProc)
{
    CCWrkStateDesc wrkStateDesc;
    CAErrno caErr;
    CCWrk *pCWrk;
    int nProcExit = CA_PROC_EXIT_OK;

    caErr = CC_Startup(&pCWrk);
    if (CA_ERR_SUCCESS != caErr)
    {
        nProcExit = CA_PROC_EXIT_FATAL;
        goto EXIT;
    }    

    CC_Touch(pCWrk);
    for (;;)
    {
        caErr = CC_State(pCWrk, TRUE, &wrkStateDesc);
        if (CA_ERR_SUCCESS != caErr)
        {
            continue;
        }

        Sleep(1000);
        if (wrkStateDesc.bIsDirty)
        {
            printf("Steat %u\n", wrkStateDesc.wrkState);
        }
    }

EXIT:
    return nProcExit;
}

static int OCAC_PWrkRun(OCACProc *pProc)
{
    CAErrno caErr;
    int nProcExit = CA_PROC_EXIT_OK;

    caErr = CA_CfgSetRTFromFile(pProc->szProcCfgFName, pProc->szProcWrkPath);
    if (CA_ERR_SUCCESS != caErr)
    {
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }    

    OCAC_PCCWrk(pProc);
EXIT:
    return nProcExit;
}

static int OCAC_PWrkStop(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    /* XXX */
    return nProcExit;
}

int OCAC_PWrk(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    switch (pProc->procType)
    {
    case OCAC_PROC_TYPE_WORKER:
        nProcExit = OCAC_PWrkRun(pProc);
        break;

    case OCAC_PROC_TYPE_KILLER:
        nProcExit = OCAC_PWrkStop(pProc);
        break;

    default:
    case OCAC_PROC_TYPE_USAGE:
        OCAC_Usage();
        nProcExit = CA_PROC_EXIT_USEAGE;
        break;
    }

    return nProcExit;
}
