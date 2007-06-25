/**
 *
 */

#include "ca_cfg.h"
#include "ca_misc.h"

CA_DECLARE(CAErrno) CA_CfgGetFName(const TCHAR *pszPath, 
                                   TCHAR *pszCfgFNameBuf, DWORD dwBufCnt)
{
    return CA_PathJoin(pszPath, CA_CFG_FNAME, pszCfgFNameBuf, dwBufCnt);
}
