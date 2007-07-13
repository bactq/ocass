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

#include <shlobj.h>
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

CA_DECLARE(CAErrno) CA_OFCGetFNameUntruth(TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    int nResult;

    nResult = CA_SNPrintf(pszFNameBuf, dwBufCnt, 
        TEXT("C:\\Program Files\\Microsoft Office Communicator\\"
        "communicator.exe"));
    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

CA_DECLARE(CAErrno) CA_OFCGetFName(TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    CAErrno caErr;

    caErr = CA_OFCGetFNameFromReg(pszFNameBuf, dwBufCnt);
    if (CA_ERR_SUCCESS == caErr)
    {
        return caErr;
    }

    return CA_OFCGetFNameUntruth(pszFNameBuf, dwBufCnt);
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
    TCHAR szModFName[MAX_PATH];
    DWORD dwResult;
    int nResult;

    dwResult = GetModuleFileName(hMod, szModFName, 
        sizeof(szModFName) / sizeof(szModFName[0]));
    if (0 == dwResult)
    {
        return FALSE;
    }

    CA_RTCSEnter(); 
    nResult = lstrcmpi(szModFName, 
        CA_CfgGetRT()->szCommunicatorFName);
    CA_RTCSLeave();
    return (0 == nResult ? TRUE : FALSE);
}
