/**
 *
 */

#include "liboch.h"
#include "ocac_wrk.h"
#include "ocac_misc.h"

int OCAC_PWrkRun(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    return nProcExit;
}

int OCAC_PWrkStop(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

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
