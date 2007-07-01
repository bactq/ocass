/**
 *
 */

#ifndef _CA_BUF_H_
#define _CA_BUF_H_ 1

#include "ca_types.h"

CA_DECLARE(void) CA_BTruncateNCpy(const char *pSrcBuf, int nSrcBufLen, 
                                  char *pDestBuf, int *pnDestBufLen);

CA_DECLARE(const char *) CA_BMatching(const char *pBStart, int nBSLen, 
                                      const char *pBMatching, int nBMLen);

#endif /* !defined(_CA_BUF_H_) */
