/**
 *
 */

#include "ca_types.h"
#include "ca_misc.h"
#include "ca_mod.h"
#include "liboch.h"
#include "ch_inner.h"

void CH_OnInject(void)
{
    HMODULE hModSpy;
    TCHAR szModPath[MAX_PATH];
    TCHAR szModSpy[MAX_PATH];
    TCHAR *pSlash;
    DWORD dwResult;
    int nResult;

    dwResult = GetModuleFileName(CH_GetDllInstance(), szModPath, 
        sizeof(szModPath) / sizeof(szModPath[0]));
    if (0 == dwResult)
    {
        return;
    }
    pSlash = strrchr(szModPath, '\\');
    if (NULL == pSlash)
    {
        return;
    }

    pSlash[0] = '\0';
    nResult = _snprintf(szModSpy, sizeof(szModSpy) / sizeof(szModSpy[0]), 
        TEXT("%s\\%s"), szModPath, CA_MOD_OCS);
    if (0 >= nResult)
    {
        return;
    }

    hModSpy = LoadLibrary(szModSpy);
    if (NULL == hModSpy)
    {
        return;
    }
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
    CHHDatum *pHDatum = CH_HDatumPtrGet();
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    DWORD dwWaitTime;
    DWORD dwThId;

    caErr = CA_GetProcFirstThread(dwProcId, &dwThId);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* XXX log */
        return caErr;
    }

    pHDatum->hInjectWndHook = SetWindowsHookEx(WH_CBT, 
            (HOOKPROC)CH_HookCBTProc, CH_GetDllInstance(), dwThId);
    if (NULL == pHDatum->hInjectWndHook)
    {
        return CA_ERR_SYS_CALL;
    }

    PostThreadMessage(dwThId, WM_NULL, 0, 0);
    /* wait for inject success */
    for (dwWaitTime = 0;;)
    {
        if (NULL == pHDatum->hInjectWndHook)
        {
            break;
        }

        Sleep(500);
        dwWaitTime += 500;
        if (0 != dwTimeOut && dwWaitTime < dwTimeOut )
        {
            funcErr = CA_ERR_TIMEOUT;
            break;
        }
    }

    return funcErr;
}
