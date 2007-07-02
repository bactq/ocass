/**
 *
 */

#include "ca_buf.h"

CA_DECLARE(void) CA_BTruncateNCpy(const char *pSrcBuf, int nSrcBufLen, 
                                  char *pDestBuf, int *pnDestBufLen)
{
    int nCpyLen = (nSrcBufLen >= *pnDestBufLen ? *pnDestBufLen : nSrcBufLen);
    memcpy(pDestBuf, pSrcBuf, nCpyLen);
    *pnDestBufLen = nCpyLen;
}

CA_DECLARE(const char *) CA_BMatching(const char *pBStart, int nBLen, 
                                      const char *pBMatching, int nBMLen)
{
    const char *pBPos;
    const char *pBMPos;
    const char *pFindPos;
    int nBOffset;
    int nBMOffset;

    for (pFindPos = NULL, pBPos = pBStart, nBOffset = 0; 
    nBOffset < nBLen; nBOffset++)
    {
        if ((nBOffset + nBMLen) > nBLen)
        {
            break;
        }

        for (pBMPos = pBMatching, nBMOffset = 0; 
        nBMOffset < nBMLen; nBMOffset++)
        {
            if ((pBPos + nBOffset + nBMOffset)[0] != 
                (pBMPos + nBMOffset)[0])
            {
                break;
            }
        }

        if (nBMOffset == nBMLen)
        {
            pFindPos = pBStart + nBOffset;
            break;
        }
    }

    return pFindPos;
}
