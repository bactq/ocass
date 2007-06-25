/**
 *
 */

#include "liboca.h"
#include "ca_inner.h"

static CARtl g_caRtl = {0};
static CARtl *g_pCARtl = NULL;

CARtl* CA_GetPtrRtl(void)
{
    return g_pCARtl;
}

CA_DECLARE(CAErrno) CA_Startup(void)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CA_Cleanup(void)
{
}
