/**
 *
 */

#include "ca_misc.h"
#include "ca_str.h"

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
    return CA_ERR_SUCCESS;
}
