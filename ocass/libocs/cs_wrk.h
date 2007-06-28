/**
 *
 */

#ifndef _CS_WRK_H_
#define _CS_WRK_H_ 1

#include "ca_types.h"

typedef struct _CSWrk
{
    HANDLE  hWrkTh;
    HANDLE  hWrkEvt;
    BOOL    bStopWrkTh;
} CSWrk;

CA_DECLARE(CAErrno) CS_WrkStart(CSWrk **pCSWrk);
CA_DECLARE(CAErrno) CS_WrkStop(CSWrk *pCSWrk);

#endif /* !defined(_CS_WRK_H_) */
