/**
 *
 */

#include "ca_misc.h"
#include "ca_str.h"
#include "ca_tlhlp.h"

CA_DECLARE(CAErrno) CA_GetModPath(HMODULE hMod, 
                                  TCHAR *pszFNameBuf, DWORD dwBufCnt)
{
    TCHAR szModFName[MAX_PATH];
    TCHAR *pSlash;
    DWORD dwResult;
    int nResult;

    dwResult = GetModuleFileName(hMod, szModFName, 
        sizeof(szModFName) / sizeof(szModFName[0]));
    if (0 == dwResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    pSlash = strrchr(szModFName, '\\');
    if (NULL == pSlash)
    {
        return CA_ERR_FNAME_BAD;
    }

    pSlash[0] = '\0';
    nResult = CA_SNPrintf(pszFNameBuf, dwBufCnt, TEXT("%s"), szModFName);
    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

CA_DECLARE(CAErrno) CA_PathJoin(const TCHAR *pszPath, 
                                const TCHAR *pszSubPath, 
                                TCHAR *pszPathBuf, DWORD dwBufCnt)
{
    int nResult;

    nResult = CA_SNPrintf(pszPathBuf, dwBufCnt, TEXT("%s\\%s"), 
        pszPath, pszSubPath);

    return (0 >= nResult ? CA_ERR_FNAME_TOO_LONG : CA_ERR_SUCCESS);
}

CA_DECLARE(void) CA_PathGetBaseName(const TCHAR *pszFullName, 
                                    const TCHAR **pszBaseName)
{
    TCHAR *pszLastSlash;

    if (NULL == pszFullName || '\0' == pszFullName[0])
    {
        *pszBaseName = pszFullName;
        return;
    }

    pszLastSlash = strrchr(pszFullName, '\\');
    if (NULL == pszLastSlash)
    {
        *pszBaseName = pszFullName;
        return;
    }

    *pszBaseName = pszLastSlash + sizeof(TCHAR);
}

CA_DECLARE(CAErrno) CA_GetProcFirstThread(DWORD dwProcId, 
                                          DWORD *pdwFirstThId)
{
    THREADENTRY32 entThread;
    BOOL bResult;
    BOOL bFind;

    CToolhelp toolHelp(TH32CS_SNAPTHREAD, dwProcId);
    entThread.dwSize = sizeof(entThread);
    bResult = toolHelp.ThreadFirst(&entThread);
    if (!bResult)
    {
        return CA_ERR_FATAL;
    }

    for (bFind = FALSE;;)
    {        
        if (entThread.th32OwnerProcessID == dwProcId)
        {
            bFind = TRUE;
            break;
        }

        bResult = toolHelp.ThreadNext(&entThread);
        if (!bResult)
        {
            break;
        }
    }
    if (!bFind)
    {
        return CA_ERR_FATAL;
    }

    *pdwFirstThId = entThread.th32ThreadID;
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_W32ResNameFromFilename(const TCHAR *pszFName, 
                        BOOL bIsGlobal, TCHAR *pszNameBuf, DWORD dwBufCnt)
{
    const TCHAR *pszPre = (bIsGlobal ? TEXT("Global\\") : TEXT("Local\\"));
    TCHAR *pszStrPos;
    int nResult;

    if (NULL == pszFName || '\0' == pszFName[0])
    {
        return CA_ERR_BAD_ARG;
    }

    nResult = CA_SNPrintf(pszNameBuf, dwBufCnt, TEXT("%s%s"), 
        pszPre, pszFName);
    if (0 >= nResult)
    {
        return CA_ERR_OBJ_NAME_TOO_LONG;
    }

    pszStrPos = pszNameBuf + lstrlen(pszPre);
    for (; '\0' != pszStrPos[0]; pszStrPos++)
    {
        if (':' == pszStrPos[0] || '\\' == pszStrPos[0] ||
            '/' == pszStrPos[0])
        {
            pszStrPos[0] = '_';
        }
    }

    return CA_ERR_SUCCESS;
}
