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

#ifndef _CA_BASE64_H_
#define _CA_BASE64_H_ 1

#include "ca_types.h"

/*
 * convert string to 
 * {A-Z,a-z,0-9,+,/},'='
 */

CA_DECLARE(int) CA_Base64EncodeLen(int nLen);
CA_DECLARE(int) CA_Base64DecodeLen(const char *pBufCoded);

CA_DECLARE(int) CA_Base64Encode(char *pEncoded, const char *pszStr, int nLen);
CA_DECLARE(int) CA_Base64Decode(char *pBufplain, const char *pBufCoded);

CA_DECLARE(int) CA_Base64DecodeBin(unsigned char *pBufPlain, 
                                   const char *pBufCoded);

CA_DECLARE(int) CA_Base64EncodeBin(char *pEncoded, const unsigned char *pStr, 
                                   int nLen);

CA_DECLARE(BOOL) CA_Base64IsEncoded(char cChar);

#endif /* !defined(_CA_BASE64_H_) */
