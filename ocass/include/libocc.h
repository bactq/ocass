/**
 *
 */

#ifndef _LIBOCC_H_
#define _LIBOCC_H_ 1

#include "liboca.h"
#include "liboch.h"

typedef struct _CC_Wrk CCWrk;

typedef enum
{
    CC_WRK_STATE_IDLE = 0,
    CC_WRK_STATE_NOT_RUNNING, 
    CC_WRK_STATE_INJECTING, 
    CC_WRK_STATE_INJECTED, 
    CC_WRK_STATE_INJECT_FAILED, 
    CC_WRK_STATE_CORPSE,
    CC_WRK_STATE_WORKING,
} CCWrkState;

typedef struct _CC_WRK_STATE_DESC
{
    CCWrkState wrkState;
    time_t startTime;
    BOOL bIsDirty;
} CCWrkStateDesc;

CA_DECLARE(CAErrno) CC_Startup(CCWrk **pCWrk);
CA_DECLARE(void)    CC_Cleanup(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, BOOL bClearDirtyFlag,
                             CCWrkStateDesc *pStateDesc);

CA_DECLARE(CAErrno) CC_StateClear(CCWrk *pCWrk);
CA_DECLARE(CAErrno) CC_Touch(CCWrk *pCWrk);

#endif /* !defined(_LIBOCC_H_) */
