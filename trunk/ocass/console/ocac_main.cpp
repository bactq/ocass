/**
 *
 */

#include "libocc.h"
#include "liboca.h"

#include "ocac_proc.h"
#include "ocac_wrk.h"

int main(int argc, char **argv)
{
    OCACProc ocacProc;
    CAErrno caErr;
    int nProcExit = CA_PROC_EXIT_OK;

    caErr = CAC_PStartup(argc, argv, &ocacProc);
    if (CA_ERR_SUCCESS != caErr)
    {
        return CA_PROC_EXIT_INIT_FAILED;
    }

    nProcExit = OCAC_PWrk(&ocacProc);
    CAC_PCleanup(&ocacProc);
    return nProcExit;
}
