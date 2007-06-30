/**
 *
 */

#ifndef _CA_BASE64_H_
#define _CA_BASE64_H_ 1

#include "ca_types.h"

CA_DECLARE(int) CA_Base64EncodeLen(int nLen);
CA_DECLARE(int) CA_Base64DecodeLen(const char *pBufCoded);

CA_DECLARE(int) CA_Base64Encode(char *pEncoded, const char *pszStr, int nLen);
CA_DECLARE(int) CA_Base64Decode(char *pBufplain, const char *pBufCoded);

CA_DECLARE(int) CA_Base64DecodeBin(unsigned char *pBufPlain, 
                                   const char *pBufCoded);

CA_DECLARE(int) CA_Base64EncodeBin(char *pEncoded, const unsigned char *pStr, 
                                   int nLen);

#endif /* !defined(_CA_BASE64_H_) */
