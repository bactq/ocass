/**
 *
 */

#include "liboca.h"
#include "ca_cfg.h"
#include "ca_misc.h"
#include "ca_ofc.h"
#include "ca_str.h"
#include "ca_inner.h"

static CACfgDatum g_rtCfgDatum = {0};

static CAErrno CA_CfgGetDefaultSetVals(const TCHAR *pszWrkPath, 
                                       CACfgDatum *pCfgDatum)
{
    CAErrno caErr;
    DWORD dwBufCnt;

    pCfgDatum->szHistoryPath[0] = '\0';
    pCfgDatum->szCommunicatorFName[0] = '\0';

    /* history dump path */
    dwBufCnt = sizeof(pCfgDatum->szHistoryPath) / 
               sizeof(pCfgDatum->szHistoryPath[0]);
    caErr = CA_PathJoin(pszWrkPath, CA_CFG_DEFAULT_HISTORY_PATH, 
        pCfgDatum->szHistoryPath, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    /* Communicator file name */
    dwBufCnt = sizeof(pCfgDatum->szCommunicatorFName) / 
               sizeof(pCfgDatum->szCommunicatorFName[0]);
    caErr = CA_OFCGetFNameFromReg(pCfgDatum->szCommunicatorFName, dwBufCnt);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszWrkPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt)
{
    return CA_PathJoin(pszWrkPath, CA_CFG_FNAME, pszCfgFNameBuf, dwBufCnt);
}

CA_DECLARE(CAErrno) CA_CfgRd(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum)
{
    CACfgDatum cfgDefaultDatum;
    CAErrno caErr;
    DWORD dwBufCnt;

    caErr = CA_CfgGetDefaultSetVals(pszWrkPath, &cfgDefaultDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    pCfgDatum->szHistoryPath[0] = '\0';
    pCfgDatum->szCommunicatorFName[0] = '\0';

    /* history dump path */
    dwBufCnt = sizeof(pCfgDatum->szHistoryPath) / 
               sizeof(pCfgDatum->szHistoryPath[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("history_path"), 
        cfgDefaultDatum.szHistoryPath, pCfgDatum->szHistoryPath, 
        dwBufCnt, pszCfgFName);

    /* Communicator file name */
    dwBufCnt = sizeof(pCfgDatum->szCommunicatorFName) /
               sizeof(pCfgDatum->szCommunicatorFName[0]);
    GetPrivateProfileString(TEXT("app"), TEXT("communicator_fname"), 
        cfgDefaultDatum.szCommunicatorFName, 
        pCfgDatum->szCommunicatorFName, dwBufCnt, pszCfgFName);

    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgWr(const TCHAR *pszCfgFName, 
                             const TCHAR *pszWrkPath, CACfgDatum *pCfgDatum)
{
    /* XXX FIXME if the path is not FULL */
    WritePrivateProfileString(TEXT("app"), TEXT("communicator_fname"), 
        pCfgDatum->szCommunicatorFName, pszCfgFName);

    WritePrivateProfileString(TEXT("app"), TEXT("history_path"), 
        pCfgDatum->szHistoryPath, pszCfgFName);
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CACfgDatum*) CA_CfgGetRT(void)
{
    return &g_rtCfgDatum;
}

CA_DECLARE(CAErrno) CA_CfgSetRT(CACfgDatum *pCfgDatum)
{
    CA_RTCSEnter();
    memcpy(&g_rtCfgDatum, pCfgDatum, sizeof(g_rtCfgDatum));
    CA_RTCSLeave();
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CfgSetRTDefault(void)
{        
    CAErrno funcErr = CA_ERR_SUCCESS;

    CA_RTCSEnter();     
    funcErr = CA_CfgGetDefaultSetVals(CA_GetPtrRT()->szRTWrkPath, 
        &g_rtCfgDatum);
    CA_RTCSLeave();
    return funcErr;
}

CA_DECLARE(CAErrno) CA_CfgSetRTFromFile(const TCHAR *pszCfgFName, 
                                        const TCHAR *pszWrkPath)
{
    CACfgDatum cfgDatum;
    CAErrno caErr;

    caErr = CA_CfgRd(pszCfgFName, pszWrkPath, &cfgDatum);
    if (CA_ERR_SUCCESS != caErr)
    {
        return caErr;
    }

    return CA_CfgSetRT(&cfgDatum);
}