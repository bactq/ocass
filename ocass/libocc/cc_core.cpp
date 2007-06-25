/**
 *
 */

#include "ca_types.h"
#include "libocc.h"
#include "cc_inner.h"

CA_DECLARE(CAErrno) CC_Startup(CCWrk **pCWrk)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CC_Cleanup(CCWrk *pCWrk)
{
}

CA_DECLARE(CAErrno) CC_State(CCWrk *pCWrk, CCWrkStateDesc *pStateDesc)
{
    return CA_ERR_SUCCESS;
}
