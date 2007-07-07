/**
 *
 */

#include "liboca.h"
#include "ca_ofc.h"
#include "ca_cfg.h"
#include "cs_core.h"
#include "cs_wrk.h"
#include "cs_misc.h"
#include "cs_nti.h"
#include "cs_log.h"
#include "cs_inner.h"

typedef struct _CS_CORE
{
    CSWrk *pCoreWrk;
} CSCore;

static CSCore g_csCore = {0};
static CSCore *g_pCSCore = NULL;

const CSWrk* CS_CoreGetWrkPtr(void)
{
    if (NULL == g_pCSCore)
    {
        return NULL;
    }

    return g_pCSCore->pCoreWrk;
}

void CS_CoreOnDllLoad(HINSTANCE hInst)
{
    /* NOP */
}

void CS_CoreOnDllUnload(HINSTANCE hInst)
{
    if (NULL == g_pCSCore)
    {
        return;
    }

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
        TEXT("Start end spy work thread."));

    /* close spy core thread */
    CS_WrkStop(g_pCSCore->pCoreWrk);

    CA_RTSetLog(NULL, NULL);
    CS_LogCleanup();
    CA_Cleanup();
}

CA_DECLARE_DYL(CAErrno) CS_Entry(HMODULE hLib, CACfgDatum *pCfgDatum)
{
    CSLogCfg caLogCfg;
    CAErrno caErr;
    BOOL bResult;

    if (NULL != g_pCSCore)
    {
        /* already startup */
        return CA_ERR_BAD_SEQ;
    }

    bResult = CS_IsRawOFCLoad(CS_GetDllInst());
    if (!bResult)
    {
        /* not communicate proc */
        return CA_ERR_BAD_SEQ;
    }

    caErr = CA_Startup();
    if (CA_ERR_SUCCESS != caErr)
    {
        /* startup failed */
        return caErr;
    }

    caErr = CA_CfgSetRT(pCfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_Cleanup();
        return caErr;
    }

    caErr = CA_RTSetLog(NULL, CS_RTLog);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_Cleanup();
        return caErr;
    }

    bResult = CA_OFCIsCommunicatorMod(NULL);
    if (!bResult)
    {
        /* not communicate proc */
        CA_Cleanup();
        return CA_ERR_BAD_SEQ;
    }

    /* startup log */
    caLogCfg.pszSpyLogFName = pCfgDatum->szSpyLog;
    caLogCfg.dwSpyLogTSize = pCfgDatum->dwSpyLogTSize;
    caLogCfg.pszNtDumpLogFName = pCfgDatum->szSpyNtDump;
    caLogCfg.dwNtDumpLogTSize = pCfgDatum->dwSpyNtDumpTSize;
    caLogCfg.spyLogMask = pCfgDatum->spyLogMask;
    CS_LogStartup(&caLogCfg);

    caErr = CS_WrkStart(hLib, pCfgDatum, &g_csCore.pCoreWrk);
    if (CA_ERR_SUCCESS != caErr)
    {
        /* start wrk thread failed */       
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Start spy work thread failed. error %u"), caErr);

        CA_Cleanup();
        return caErr;
    }

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
        TEXT("Start spy work thread successed."));

    /* attach network api */
    CS_NtWrkApiAttach();
    g_pCSCore = &g_csCore;
    return CA_ERR_SUCCESS;
}
