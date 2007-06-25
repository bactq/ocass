/**
 *
 */

#ifndef _LIBOCC_H_
#define _LIBOCC_H_ 1

#include "liboca.h"
#include "liboch.h"

typedef struct _CC_Wrk CCWrk;

typedef struct _CC_WRK_STATE_DESC
{    
    time_t startTime;
} CCWrkStateDesc;

CA_DECLARE(CAErrno) CC_Startup(CCWrk **pCWrk);
CA_DECLARE(void)    CC_Cleanup(CCWrk *pCWrk);
CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, CCWrkStateDesc *pStateDesc);

#endif /* !defined(_LIBOCC_H_) */
