/**
 *
 */

#include "ca_types.h"

static HINSTANCE g_hInst = NULL;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hInst;
        break;
    case DLL_PROCESS_DETACH:
        g_hInst = NULL;
        break;
    default:
        break;
    }

    return TRUE;
}
