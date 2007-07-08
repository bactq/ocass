/*
 * Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 * This file is part of ocass
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

CA_DECLARE(const char *) CA_BMatchingAlpha(const char *pBStart, int nBLen, 
                                           const char *pszBMatching, 
                                           BOOL bSensitive)
{
    const char *pBPos;
    const char *pBMPos;
    const char *pFindPos;
    const int nBMLen = strlen(pszBMatching);
    char cLower;
    char cUpper;
    char cPos;
    int nBMOffset;
    int nBOffset;

    for (pFindPos = NULL, pBPos = pBStart, nBOffset = 0; 
    nBOffset < nBLen; nBOffset++)
    {
        if ((nBOffset + nBMLen) > nBLen)
        {
            break;
        }

        for (pBMPos = pszBMatching, nBMOffset = 0; 
        nBMOffset < nBMLen; nBMOffset++)
        {
            cPos = (pBMPos + nBMOffset)[0];
            if (bSensitive)
            {
                cLower = cPos;
                cUpper = cPos;
            }
            else
            {
                /* 'a' == 97
                 * 'A' == 65
                 */
                if ('A' <= cPos && 'Z' >= cPos)
                {
                    /* cPos is upper */
                    cUpper = cPos;
                    cLower = cPos + ('a' - 'A');
                }
                else if ('a' <= cPos && 'z' >= cPos)
                {
                    /* cPos is lower */
                    cLower = cPos;
                    cUpper = cLower - ('a' - 'A');
                }
                else
                {
                    cLower = cPos;
                    cUpper = cPos;
                }
            }

            if ((pBPos + nBOffset + nBMOffset)[0] != cLower && 
                (pBPos + nBOffset + nBMOffset)[0] != cUpper)
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
