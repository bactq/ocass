/**
 *
 */

#include "ca_str.h"


CA_DECLARE(CHAR *) CA_StrTok(CHAR *pszStr, const CHAR *pszSep, CHAR **pszLast)
{
    CHAR *pszToken;

    if (!pszStr)           /* subsequent call */       
        pszStr = *pszLast;    /* start where we left off */

    /* skip characters in sep (will terminate at '\0') */
    while (*pszStr && strchr(pszSep, *pszStr))
        ++pszStr;

    if (!*pszStr)          /* no more tokens */
        return NULL;

    pszToken = pszStr;

    /* skip valid token characters to terminate token and
     * prepare for the next call (will terminate at '\0) 
     */
    *pszLast = pszToken + 1;
    while (**pszLast && !strchr(pszSep, **pszLast))
        ++*pszLast;

    if (**pszLast) {
        **pszLast = '\0';
        ++*pszLast;
    }

    return pszToken;
}

CA_DECLARE_NOSTD(int) CA_SNPrintf(TCHAR *pszBuf, const int nBufCnt, 
                                  const TCHAR *pszFmt, ...)
{
    va_list pArgList;
    int nResult;

    va_start(pArgList, pszFmt);
    nResult = _vsnprintf(pszBuf, nBufCnt, pszFmt, pArgList);
    va_end(pArgList);
    return nResult;
}


CA_DECLARE(int) CA_TruncateStrnCpy(char *pszDst, 
                    const char *pszSrc, int nDstSize)
{
    char *pStrPos;
    int i;

    /* XXX if ((signed)nDstSize =< 0) then call assert function */

    pszDst[0] = '\0';
    if (NULL == pszSrc)
    {
        return 0;
    }

    pStrPos = (char *)pszSrc;
    i = 0;
    for (; !('\0' == pStrPos[0] || i >= nDstSize); pStrPos++, i++)
    {
        pszDst[i] = pStrPos[0];
    }

    pszDst[i] = '\0';
    return strlen(pszDst);
}

CA_DECLARE(void) CA_BTruncateNCpy(const char *pSrcBuf, int nSrcBufLen, 
                                  char *pDestBuf, int *pnDestBufLen)
{
    int nCpyLen = (nSrcBufLen >= *pnDestBufLen ? *pnDestBufLen : nSrcBufLen);
    memcpy(pDestBuf, pSrcBuf, nCpyLen);
    *pnDestBufLen = nCpyLen;
}
