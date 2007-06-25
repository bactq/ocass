/**
 *
 */

#ifndef _CA_MM_H_
#define _CA_MM_H_ 1

#include "ca_types.h"

CA_DECLARE(void*)   CA_MAlloc(UINT nSize);
CA_DECLARE(void)    CA_MFree(void *pMem);

#endif /* !defined(_CA_MM_H_) */
