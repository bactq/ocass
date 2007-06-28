/**
 *
 */

#ifndef _CA_INNER_H_
#define _CA_INNER_H_ 1

#include "ca_types.h"
#include "ca_mm.h"
#include "ca_flock.h"

typedef struct _CART
{
    HINSTANCE hCADll;
    TCHAR szRTWrkPath[MAX_PATH];
    CRITICAL_SECTION rtCS;
    CAFLock rtPL;
} CART;

CART* CA_GetPtrRT(void);

const HINSTANCE CA_GetDllInstance(void);

#endif /* !defined(_CA_INNER_H_) */

