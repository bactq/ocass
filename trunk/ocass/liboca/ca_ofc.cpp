/**
 *
 */

#include "liboca.h"
#include "ca_ofc.h"
#include "ca_mod.h"
#include "ca_str.h"
#include "ca_misc.h"
#include "ca_cfg.h"
#include "ca_tlhlp.h"
#include "ca_inner.h"

static BOOL CA_OFCIsOFCProc(PROCESSENTRY32 *pProcEnt, 
                            TCHAR *pszOFCExeFName)
{
    MODULEENTRY32 entMod;
    const TCHAR *pszBaseName;
    BOOL bResult;
    int nResult;

    CA_PathGetBaseName(pszOFCExeFName, &pszBaseName);
    CToolhelp toolHelp(TH32CS_SNAPMODULE, pProcEnt->th32ProcessID);
    entMod.dwSize = sizeof(entMod);
    bResult = toolHelp.ModuleFind((TCHAR *)pszBaseName, &entMod);
    if (!bResult)
    {
        return FALSE;
    }

    nResult = lstrcmpi(pszOFCExeFName, entMod.szExePath);
    if (0 != nResult)
    {
        return FALSE;
    }

    return TRUE;
}

CA_DECLARE(CAErrno) CA_OFCGetFNameFromReg(TCHAR *pszFNameBuf, 
                                          DWORD dwBufCnt)
{
    const TCHAR *pszMSOCExe = CA_OFC_EXE_BASE_NAME;
    CAErrno funcErr = CA_ERR_SUCCESS;
    TCHAR szInstallDir[MAX_PATH];
    DWORD dwDataLen;
    HKEY hKeyInstallDir = NULL;
    LONG nResult;
    
    nResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
        TEXT("SOFTWARE\\Microsoft\\Communicator"), 
        0, KEY_READ, &hKeyInstallDir);
    if (ERROR_SUCCESS != nResult)
    {
        hKeyInstallDir = NULL;
        funcErr = CA_ERR_FNAME_CANNOT_FIND;
        goto EXIT;
    }

    szInstallDir[0] = '\0';
    dwDataLen = sizeof(szInstallDir) / sizeof(szInstallDir[0]);   
    nResult = RegQueryValueEx(hKeyInstallDir, 
        TEXT("InstallationDirectory"), NULL, NULL, 
        (LPBYTE)szInstallDir, &dwDataLen);
    if (ERROR_SUCCESS != nResult)
    {
        funcErr = CA_ERR_FNAME_CANNOT_FIND;
        goto EXIT;
    }

    /* remove last slash */
    if (szInstallDir[lstrlen(szInstallDir) - 1] == '\\')
    {
        szInstallDir[lstrlen(szInstallDir) - 1] = '\0';
    }


    funcErr = CA_PathJoin(szInstallDir, pszMSOCExe, pszFNameBuf, dwBufCnt);
EXIT:
    if (NULL != hKeyInstallDir)
    {
        RegCloseKey(hKeyInstallDir);
    }
    return funcErr;
}

CA_DECLARE(CAErrno) CA_OFCGetRunProcId(DWORD *pdwProcId)
{
    PROCESSENTRY32 entProc;
    TCHAR szOFCExeFName[MAX_PATH];
    BOOL bResult;
    BOOL bFind;
    int nResult;

    *pdwProcId = 0;
    CA_RTCSEnter();
    nResult = CA_SNPrintf(szOFCExeFName, 
        sizeof(szOFCExeFName) / sizeof(szOFCExeFName[0]), 
        TEXT("%s"), CA_CfgGetRT()->szCommunicatorFName);
    CA_RTCSLeave();
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    CToolhelp toolHelp(TH32CS_SNAPPROCESS, 0);
    entProc.dwSize = sizeof(entProc);
    bResult = toolHelp.ProcessFirst(&entProc);
    if (!bResult)
    {
        return CA_ERR_OBJ_CANNOT_FIND;
    }

    for (bFind = FALSE;;)
    {
        bResult = CA_OFCIsOFCProc(&entProc, szOFCExeFName);
        if (bResult)
        {
            bFind = TRUE;
            break;
        }

        bResult = toolHelp.ProcessNext(&entProc);
        if (!bResult)
        {
            break;
        }
    }
    if (!bFind)
    {
        return CA_ERR_OBJ_CANNOT_FIND;
    }

    *pdwProcId = entProc.th32ProcessID;
    return CA_ERR_SUCCESS;
}

CA_DECLARE(BOOL) CA_OFCProcIsAttached(DWORD dwProcId)
{
    MODULEENTRY32 entMod;
    BOOL bResult;
    
    CToolhelp toolHelp(TH32CS_SNAPMODULE, dwProcId);
    entMod.dwSize = sizeof(entMod);
    bResult = toolHelp.ModuleFind(CA_MOD_OCS, &entMod);
    if (!bResult)
    {
        return FALSE;
    }

    return TRUE;
}

CA_DECLARE(BOOL) CA_OFCIsCommunicatorMod(HMODULE hMod)
{
    return FALSE;
}
