/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
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
    CA_RTSetLogFilter(NULL, NULL);
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

    CA_RTSetLog(NULL, CS_RTLog);
    CA_RTSetLogFilter(NULL, CS_RTLogFilter);

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

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
        TEXT("Start spy work thread successed."));

    /* attach network api */
    CS_NtWrkApiAttach();
    g_pCSCore = &g_csCore;
    return CA_ERR_SUCCESS;
}
