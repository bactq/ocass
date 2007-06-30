/**
 *
 */

#include "liboch.h"
#include "ca_cfg.h"
#include "ca_evts.h"
#include "ocac_wrk.h"
#include "ocac_misc.h"

static void OCAC_PCCWrkShowState(OCACProc *pProc, 
                                 CCWrk *pCWrk)
{
    CCWrkStateDesc wrkStateDesc;
    CAErrno caErr;

    caErr = CC_State(pCWrk, TRUE, &wrkStateDesc);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Read core state failed. Err %u"), caErr);
        return;
    }

    if (!wrkStateDesc.bIsDirty)
    {
        return;
    }

    OCAC_PrintCoreState(&wrkStateDesc);
}

static int OCAC_PCCWrk(OCACProc *pProc)
{
    CAErrno caErr;
    HANDLE hCREvt = NULL;
    CCWrk *pCWrk;
    DWORD dwResult;
    int nProcExit = CA_PROC_EXIT_OK;

    hCREvt = OpenEvent(EVENT_ALL_ACCESS, TRUE, OCASS_EVT_NAME_SPY_RUN);
    if (NULL != hCREvt)
    {
        nProcExit = CA_PROC_EXIT_ALREADY_RUN;
        CloseHandle(hCREvt);

        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Core already startup"));
        goto EXIT;
    }

    hCREvt = CreateEvent(NULL, FALSE, FALSE, OCASS_EVT_NAME_SHELL_WRK);
    if (NULL == hCREvt)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create core event (%s) failed. system error %u."), 
            OCASS_EVT_NAME_SHELL_WRK, GetLastError());
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }

    caErr = CC_Startup(&pCWrk);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Core startup failed. Error %u."), caErr);

        nProcExit = CA_PROC_EXIT_FATAL;
        goto EXIT;
    }

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
            TEXT("Core startup success"));

    CC_Touch(pCWrk);
    for (;;)
    {
        dwResult = WaitForSingleObject(hCREvt, 1000 * 2);
        if (WAIT_TIMEOUT == dwResult)
        {
            OCAC_PCCWrkShowState(pProc, pCWrk);
        }
        else if (WAIT_FAILED == dwResult)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_WARN, 
                TEXT("Wait core work event failed. system error %u."), 
                GetLastError());

            Sleep(1000);
            OCAC_PCCWrkShowState(pProc, pCWrk);
        }
        else
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
                TEXT("Recive core shutdown event"));
            break;
        }
    }

    CA_RTLog(CA_SRC_MARK, CA_RTLOG_INFO, 
        TEXT("Work process exit (%u)"), nProcExit);
EXIT:
    if (NULL != hCREvt)
    {
        CloseHandle(hCREvt);
    }
    return nProcExit;
}

static int OCAC_PWrkRun(OCACProc *pProc)
{
    CAErrno caErr;
    int nProcExit = CA_PROC_EXIT_OK;

    caErr = CA_CfgSetRTFromFile(pProc->szProcCfgFName, pProc->szProcWrkPath);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Read config file failed. "
            "cfg file (%s), wrk path (%s). error %u"), 
            pProc->szProcCfgFName, pProc->szProcWrkPath, 
            caErr);
        nProcExit = CA_PROC_EXIT_INIT_FAILED;
        goto EXIT;
    }    

    OCAC_PCCWrk(pProc);
EXIT:
    return nProcExit;
}

static int OCAC_PWrkStop(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    /* XXX */
    return nProcExit;
}

int OCAC_PWrk(OCACProc *pProc)
{
    int nProcExit = CA_PROC_EXIT_OK;

    switch (pProc->procType)
    {
    case OCAC_PROC_TYPE_WORKER:
        nProcExit = OCAC_PWrkRun(pProc);
        break;

    case OCAC_PROC_TYPE_KILLER:
        nProcExit = OCAC_PWrkStop(pProc);
        break;

    default:
    case OCAC_PROC_TYPE_USAGE:
        OCAC_Usage();
        nProcExit = CA_PROC_EXIT_USEAGE;
        break;
    }

    return nProcExit;
}
