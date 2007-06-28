/**
 *
 */

#ifndef _CS_INNER_H_
#define _CS_INNER_H_ 1

#include "liboca.h"
#include "ca_types.h"

typedef struct _CSWrkDatum
{
    HANDLE hWrkTh;
} CSWrkDatum;

const HINSTANCE CS_GetDllInst(void);

#endif /* !defined(_CS_INNER_H_) */
