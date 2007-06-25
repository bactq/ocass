/**
 *
 */

#ifndef _CA_GETOPT_H_
#define _CA_GETOPT_H_ 1

#include "ca_types.h"

typedef struct _CA_GETOPT_DATUM 
{
    int nArgc;
    char **pArgv;
    int nScanArgvPos;
} CAGetoptDatum;

CA_DECLARE(void) CA_InitGetOpt(int argc, char **argv, 
                               CAGetoptDatum *pDatum);

CA_DECLARE(CAErrno) CA_GetOpt(CAGetoptDatum *pDatum, const char *pszOpts, 
                              char *pOptCh, char **pszOptArg);

#endif /* !defined(_CA_GETOPT_H_) */
