/**
 *
 */

#include "ca_types.h"
#include "liboch.h"
#include "ch_inner.h"

void CH_OnInject(void)
{
    /* load spy library */
}

void CH_OnInjectComplete(CHHDatum *pHDatum)
{
    if (NULL == pHDatum || NULL == pHDatum->hInjectWndHook)
    {
        return;
    }

    /* unhook this proc */
    UnhookWindowsHookEx(pHDatum->hInjectWndHook);
    pHDatum->hInjectWndHook = NULL;
}

CA_DECLARE(CAErrno) CH_PInject(DWORD dwProcId, DWORD dwTimeOut)
{
    /* set hook */
    return CA_ERR_SUCCESS;
}
