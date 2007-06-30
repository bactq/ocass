/**
 *
 */

#ifndef _CC_INNER_H_
#define _CC_INNER_H_ 1

#include "libocc.h"

struct _CC_Wrk
{
    HANDLE              hWrkTh;
    CRITICAL_SECTION    wrkCS;
    HANDLE              hWrkEvt;
    BOOL                bStopWrkTh;

    CCWrkMod    wrkMod;
    BOOL        bStateDirty;
    CCWrkState  wrkState;
    time_t      stateStartTime;
};


DWORD WINAPI CC_WrkThread(void *pThArg);

#endif /* !defined(_CC_INNER_H_) */

