/**
 *
 */

#include "ca_ofc.h"
#include "cs_misc.h"

BOOL CS_IsRawOFCLoad(HINSTANCE hInst)
{
    TCHAR szProcName[MAX_PATH];
    TCHAR *pszBaseName;
    DWORD dwResult;
    int nResult;
    
    dwResult = GetModuleFileName(NULL, szProcName, 
        sizeof(szProcName) / sizeof(szProcName[0]));
    if (0 == dwResult)
    {
        return FALSE;
    }

    pszBaseName = strrchr(szProcName, '\\');
    if (NULL == pszBaseName)
    {
        return FALSE;
    }
    pszBaseName++;

    nResult = lstrcmpi(pszBaseName, CA_OFC_EXE_BASE_NAME);
    return (0 == nResult ? TRUE : FALSE);
}
