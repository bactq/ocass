/**
 *
 */

#include "liboca.h"
#include "ca_misc.h"
#include "ocac_misc.h"

void OCAC_VPrintMsg(const TCHAR *pszMsgFmt, va_list pArgList)
{
    FILE *pMsgFd = stdout;
    vfprintf(pMsgFd, pszMsgFmt, pArgList);
}

void OCAC_VPrintMsgLine(const TCHAR *pszMsgFmt, va_list pArgList)
{
    OCAC_VPrintMsg(pszMsgFmt, pArgList);
    OCAC_PrintMsg(TEXT("\n"));
}

void OCAC_PrintMsg(const TCHAR *pszMsgFmt, ...)
{
    va_list pArgList;
    va_start(pArgList, pszMsgFmt);
    OCAC_VPrintMsg(pszMsgFmt, pArgList);
    va_end(pArgList);
}

void OCAC_PrintMsgLine(const TCHAR *pszMsgFmt, ...)
{
    va_list pArgList;
    va_start(pArgList, pszMsgFmt);
    OCAC_VPrintMsgLine(pszMsgFmt, pArgList);
    va_end(pArgList);
}

void OCAC_LogShow(void *pCbCtx, CARTLog *pLog)
{
    const TCHAR *pszLogType;
    CAErrno caErr;
    TCHAR szTm[CA_TM_STR_MAX_BUF];

    if (NULL == pLog || NULL == pLog->pszLog || '\0' == pLog->pszLog[0])
    {
        return;
    }

    caErr = CA_GetCurTimeStr(szTm, sizeof(szTm) / sizeof(szTm[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTm[0] = '\0';
    }
    pszLogType = CA_RTLogFlagsDesc(pLog->logFlags);

    OCAC_PrintMsg(TEXT("%s [%s] [%s(%u)] %s\n"), szTm, pszLogType, 
        NULL == pLog->pszSrcBase ? TEXT("") : pLog->pszSrcBase, 
        pLog->nSrcLine, pLog->pszLog);
}

void OCAC_Panic(const TCHAR *pszSrc, const int nSrcLine, 
                const TCHAR *pszFmt, ...)
{
    const TCHAR *pszSrcBase;
    va_list pArgList;

    CA_PathGetBaseName(pszSrc, &pszSrcBase);
    OCAC_PrintMsg(TEXT("PANIC [%s(%u)]: "), 
        NULL == pszSrcBase ? TEXT("") : pszSrcBase, nSrcLine);

    va_start(pArgList, pszFmt);
    OCAC_VPrintMsgLine(pszFmt, pArgList);
    va_end(pArgList);
}

void OCAC_Usage(void)
{
    OCAC_PrintMsg(TEXT("Useage: \n"));
}

void OCAC_PrintCoreState(CCWrkStateDesc *pWrkState)
{
    const TCHAR *pszWrkState = CC_StateDesc(pWrkState->wrkState);
    CAErrno caErr;
    TCHAR szTm[CA_TM_STR_MAX_BUF];

    caErr = CA_GetTimeStr(pWrkState->startTime, szTm, 
        sizeof(szTm) / sizeof(szTm[0]));
    if (CA_ERR_SUCCESS != caErr)
    {
        szTm[0] = '\0';
    }

    OCAC_PrintMsgLine(TEXT("Core State: %s %s"), szTm, pszWrkState);
}
