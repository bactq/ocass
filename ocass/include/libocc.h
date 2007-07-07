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
    CC_WRK_STATE_NOT_INJECTED, 
    CC_WRK_STATE_INJECTING, 
    CC_WRK_STATE_INJECTED, 
    CC_WRK_STATE_INJECT_FAILED, 
    CC_WRK_STATE_CORPSE,
    CC_WRK_STATE_WORKING,
} CCWrkState;

typedef int CCWrkMod;
#define CC_WRK_MOD_DETECT       (1<< 0)
#define CC_WRK_MOD_INJECT       (1<< 1)
#define CC_WRK_MOD_PAUSE        (1<< 2)

#define CC_WRK_MOD_NORMAL       (CC_WRK_MOD_DETECT|CC_WRK_MOD_INJECT)
#define CC_WRK_MOD_MON          (CC_WRK_MOD_DETECT)
#define CC_WRK_MOD_SAFE         (CC_WRK_MOD_PAUSE)

typedef struct _CC_WRK_STATE_DESC
{
    CCWrkState wrkState;
    time_t startTime;

    CCWrkMod wrkMod;
    BOOL bIsDirty;
} CCWrkStateDesc;

CA_DECLARE(CAErrno) CC_Startup(CCWrk **pCWrk);
CA_DECLARE(void)    CC_Cleanup(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, BOOL bClearDirtyFlag,
                             CCWrkStateDesc *pStateDesc);

CA_DECLARE(CAErrno) CC_SetWrkMod(CCWrk *pCWrk, CCWrkMod wrkMod);
CA_DECLARE(CAErrno) CC_GetWrkMod(CCWrk *pCWrk, CCWrkMod *pWrkMod);
CA_DECLARE(CAErrno) CC_SetPauseFlag(CCWrk *pCWrk, BOOL bPause);
CA_DECLARE(CAErrno) CC_UpdateCfg(CCWrk *pCWrk, BOOL bPause);

CA_DECLARE(CAErrno) CC_StateClear(CCWrk *pCWrk);

CA_DECLARE(CAErrno) CC_Touch(CCWrk *pCWrk);

CA_DECLARE(const TCHAR*) CC_StateDesc(CCWrkState wrkState);

#endif /* !defined(_LIBOCC_H_) */
