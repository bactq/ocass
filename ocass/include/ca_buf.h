/**
 *
 */

#ifndef _CA_BUF_H_
#define _CA_BUF_H_ 1

#include "ca_types.h"

CA_DECLARE(void) CA_BTruncateNCpy(const char *pSrcBuf, int nSrcBufLen, 
                                  char *pDestBuf, int *pnDestBufLen);

#endif /* !defined(_CA_BUF_H_) */
