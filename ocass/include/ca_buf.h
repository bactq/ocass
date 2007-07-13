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

#ifndef _CA_BUF_H_
#define _CA_BUF_H_ 1

#include "ca_types.h"

CA_DECLARE(void) CA_BTruncateNCpy(const char *pSrcBuf, int nSrcBufLen, 
                                  char *pDestBuf, int *pnDestBufLen);

CA_DECLARE(const char *) CA_BMatching(const char *pBStart, int nBLen, 
                                      const char *pBMatching, int nBMLen);

CA_DECLARE(const char *) CA_BMatchingAlpha(const char *pBStart, int nBLen, 
                                           const char *pszBMatching, 
                                           BOOL bSensitive);

#endif /* !defined(_CA_BUF_H_) */
