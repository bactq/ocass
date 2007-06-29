/**
 *
 */

#ifndef _CA_MM_H_
#define _CA_MM_H_ 1

#include "ca_types.h"

CA_DECLARE(void*)   CA_MAlloc(UINT nSize);
CA_DECLARE(void)    CA_MFree(void *pMem);

/**
 * @{
 */

typedef struct _CA_SHMM_ CAShMM;

CA_DECLARE(CAErrno) CA_ShMMCreate(UINT nReqSize, const TCHAR *pszFName, 
                                 CAShMM **pShMM);

CA_DECLARE(void)    CA_ShMMDestroy(CAShMM *pShMM, BOOL bUnlinkMapFile);

CA_DECLARE(CAErrno) CA_ShMMAttach(const TCHAR *pszFileName, CAShMM **pShm);
CA_DECLARE(void)    CA_ShMMDettach(CAShMM *pShm);

CA_DECLARE(void *) CA_ShMMBaseAddrGet(CAShMM *pShm);
CA_DECLARE(UINT) CA_ShMMSizeGet(CAShMM *pShm);

/** @} */

#endif /* !defined(_CA_MM_H_) */
