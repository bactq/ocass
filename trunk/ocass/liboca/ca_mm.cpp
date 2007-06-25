/**
 *
 */

#include "ca_mm.h"

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
