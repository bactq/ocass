/**
 *
 */

#include "liboca.h"
#include "ca_ofc.h"
#include "cs_core.h"
#include "cs_wrk.h"
#include "cs_misc.h"
#include "cs_nti.h"

typedef struct _CS_CORE
{
    CSWrk *pCoreWrk;
} CSCore;

static CSCore g_csCore = {0};
static CSCore *g_pCSCore = NULL;

void CS_CoreOnDllLoad(HINSTANCE hInst)
{
    CAErrno caErr;
    BOOL bResult;

    if (NULL != g_pCSCore)
    {
        /* already startup */
        return;
    }

    bResult = CS_IsRawOFCLoad(hInst);
    if (!bResult)
    {
        /* not communicate proc */
        return;
    }

    caErr = CA_Startup();
    if (CA_ERR_SUCCESS != caErr)
    {
        /* startup failed */
        return;
    }

    bResult = CA_OFCIsCommunicatorMod(NULL);
    if (!bResult)
    {
        /* not communicate proc */
        CA_Cleanup();
        return;
    }

    caErr = CS_WrkStart(&g_csCore.pCoreWrk);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* start wrk thread failed */
        CA_Cleanup();
        return;
    }

    /* attach network api */
    CS_NtWrkApiAttach();
    g_pCSCore = &g_csCore;
}

void CS_CoreOnDllUnload(HINSTANCE hInst)
{
}
