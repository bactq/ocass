/**
 *
 */

#include "ca_types.h"
#include "ch_inner.h"

LRESULT WINAPI CH_HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    CHHDatum *pHDatum = CH_HDatumPtrGet();
    LRESULT nResult;

    if (NULL == pHDatum || NULL == pHDatum->hInjectWndHook)
    {
        return -1;
    }

    nResult = CallNextHookEx(pHDatum->hInjectWndHook, nCode, wParam, lParam);

    CH_OnInject(pHDatum);
    CH_OnInjectComplete(pHDatum);
    return nResult;
}
