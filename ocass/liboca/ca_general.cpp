/**
 *
 */

#include "liboca.h"
#include "ca_inner.h"
#include "ca_cfg.h"
#include "ca_misc.h"

static CART g_caRT = {0};
static CART *g_pCART = NULL;

CART* CA_GetPtrRT(void)
{
    return g_pCART;
}

static CAErrno CA_RTStartup(void)
{
    CRITICAL_SECTION *pRTCS = NULL;
    CAFLock *pRTPL = NULL;
    CAErrno caErr;
    CAErrno funcErr = CA_ERR_SUCCESS;
    DWORD dwBufCnt;
    TCHAR szRTFLock[MAX_PATH];

    g_caRT.hCADll = CA_GetDllInstance();
    dwBufCnt = sizeof(g_caRT.szRTWrkPath) / sizeof(g_caRT.szRTWrkPath[0]);
    caErr = CA_GetModPath(g_caRT.hCADll, g_caRT.szRTWrkPath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    dwBufCnt = sizeof(szRTFLock) / sizeof(szRTFLock[0]);
    caErr = CA_PathJoin(g_caRT.szRTWrkPath, CA_RT_PL_FNAME, 
        szRTFLock, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    InitializeCriticalSection(&g_caRT.rtCS);
    pRTCS = &g_caRT.rtCS;

    caErr = CA_FLockCreate(szRTFLock, &g_caRT.rtPL);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }
    pRTPL = &g_caRT.rtPL;

EXIT:
    if (CA_ERR_SUCCESS == funcErr)
    {
        g_pCART = &g_caRT;
    }
    else
    {
        g_caRT.hCADll = NULL;
        g_caRT.szRTWrkPath[0] = '\0';

        if (NULL != pRTCS)
        {
            DeleteCriticalSection(pRTCS);
        }

        if (NULL != pRTPL)
        {
            CA_FLockDestroy(pRTPL, FALSE);
        }
    }

    return funcErr;
}

static CAErrno CA_RTCleanup(void)
{
    g_pCART = NULL;

    g_caRT.hCADll = NULL;
    g_caRT.szRTWrkPath[0] = '\0';
    DeleteCriticalSection(&g_caRT.rtCS);
    CA_FLockDestroy(&g_caRT.rtPL, FALSE);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_Startup(void)
{
    CAErrno caErr;

    caErr = CA_RTStartup();
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* set default config */
    caErr = CA_CfgSetRTDefault();
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTCleanup();
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(void) CA_Cleanup(void)
{
    CA_RTCleanup();
}

CA_DECLARE(CAErrno) CA_RTCSEnter(void)
{
    EnterCriticalSection(&(CA_GetPtrRT()->rtCS));
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_RTCSLeave(void)
{
    LeaveCriticalSection(&(CA_GetPtrRT()->rtCS));
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_RTPLockEnter(CAFLockOpt lockOpt)
{
    return CA_FLockEnter(&(CA_GetPtrRT()->rtPL), lockOpt);
}

CA_DECLARE(CAErrno) CA_RTPLockLeave(void)
{
    return CA_FLockLeave(&(CA_GetPtrRT()->rtPL));
}
