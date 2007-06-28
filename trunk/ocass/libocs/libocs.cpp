/**
 *
 */

#include "ca_types.h"
#include "cs_inner.h"
#include "cs_core.h"

static HINSTANCE g_hInst = NULL;

CA_DECLARE(void) Sanctimonious(void)
{
    /* nop */
}

const HINSTANCE CS_GetDllInst(void)
{
    return g_hInst;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hInst;
        CS_CoreOnDllLoad(hInst);
        break;
    case DLL_PROCESS_DETACH:
        CS_CoreOnDllUnload(hInst);
        g_hInst = NULL;
        break;
    default:
        break;
    }

    return TRUE;
}
