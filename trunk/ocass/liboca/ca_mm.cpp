/**
 *
 */

#include "ca_mm.h"

typedef struct _CA_ShmBlk
{
    DWORD nShmRealSize;
    DWORD nShmSize;
} CAShmBlk;

struct _CA_SHMM_
{  
    TCHAR szFileName[MAX_PATH];
    DWORD nShmSize;

    HANDLE hMap;
    LPVOID pShmBase;
    CAShmBlk *pShmBlk;
};

CA_DECLARE(void*) CA_MAlloc(UINT nSize)
{
    return malloc(nSize);
}

CA_DECLARE(void) CA_MFree(void *pMem)
{
    if (NULL == pMem)
    {
        return;
    }

    free(pMem);
}

CA_DECLARE(CAErrno) CA_ShMMCreate(UINT nReqSize, const TCHAR *pszFName, 
                                 CAShMM **pShMM)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_ShMMDestroy(CAShMM *pShMM, BOOL bUnlinkMapFile)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_ShMMAttach(const char *pszFileName, CAShMM **phShm)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_ShMMDettach(CAShMM *phShm)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(void *) CA_ShMMBaseAddrGet(CAShMM *phShm)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(UINT) CA_ShMMSizeGet(CAShMM *phShm)
{
    return CA_ERR_SUCCESS;
}
