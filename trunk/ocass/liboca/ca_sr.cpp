/**
 *
 */

#include "ca_sr.h"
#include "ca_flock.h"
#include "ca_mm.h"

struct _CA_SR
{
    BOOL bIsAttach;

    CAFLock spyFLock;
    CAShMM *pSHMM;
    CASpyDatum *pSpyDatum;
};


CA_DECLARE(CAErrno) CA_SRCreate(CASpyRun **pSR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_SRAttach(CASpyRun **pSR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_SRClose(CASpyRun *pSR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_SRLock(CASpyRun *pSR, BOOL bWr)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_SRUnlock(CASpyRun *pSR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CASpyDatum*) CA_SRGetDatum(CASpyRun *pSR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CASpyDatum*) CA_SRTouch(CASpyRun *pSR)
{
    return CA_ERR_SUCCESS;
}

