/**
 *
 */

#include "cs_wrk.h"

DWORD WINAPI CS_WrkTh(void *pArg)
{
    return CA_THREAD_EXIT_OK; 
}


CA_DECLARE(CAErrno) CS_WrkStart(CSWrk **pCSWrk)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CS_WrkStop(CSWrk *pCSWrk)
{
    return CA_ERR_SUCCESS;
}
