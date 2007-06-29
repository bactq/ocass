/**
 *
 */

#include "cs_wrk.h"

struct _CSWrk
{
    HANDLE  hWrkTh;
    HANDLE  hWrkEvt;
    BOOL    bStopWrkTh;
};

DWORD WINAPI CS_WrkTh(void *pArg)
{
    return CA_THREAD_EXIT_OK; 
}


CAErrno CS_WrkStart(HMODULE hLib, CACfgDatum *pCfgDatum, 
                    CSWrk **pCSWrk)
{
    return CA_ERR_SUCCESS;
}

CAErrno CS_WrkStop(CSWrk *pCSWrk)
{
    return CA_ERR_SUCCESS;
}
