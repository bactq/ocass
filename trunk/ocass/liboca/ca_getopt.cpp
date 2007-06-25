/**
 *
 */

#include "ca_getopt.h"


CA_DECLARE(void) CA_InitGetOpt(int argc, char **argv, 
                               CAGetoptDatum *pDatum)
{
    pDatum->nArgc = argc;
    pDatum->pArgv = argv;
    pDatum->nScanArgvPos = 1;
}

CA_DECLARE(CAErrno) CA_GetOpt(CAGetoptDatum *pDatum, const char *pszOpts, 
                              char *pOptCh, char **pszOptArg)
{
    char *pOptsStrPos;
    char chOpt;
 
    if (NULL == pszOpts || '\0' == pszOpts[0]) 
    {
        return CA_ERR_EOF;
    }

    if ((UINT)pDatum->nArgc <= (UINT)pDatum->nScanArgvPos)
    {
        /* scan command end */
        return CA_ERR_EOF;
    }

    if (2 != strlen(pDatum->pArgv[pDatum->nScanArgvPos]) || 
        ('-' != pDatum->pArgv[pDatum->nScanArgvPos][0] && 
         '/' != pDatum->pArgv[pDatum->nScanArgvPos][0]))
    {
        return CA_ERR_BAD_ARG;
    }

    chOpt = pDatum->pArgv[pDatum->nScanArgvPos][1];

    for (pOptsStrPos = (char *)pszOpts; '\0' != pOptsStrPos[0]; pOptsStrPos++)
    {
        if (':' == pOptsStrPos[0]) 
        {
            continue;
        }

        if (pOptsStrPos[0] == chOpt)
        {
            break;
        }
    }

    if ('\0' == pOptsStrPos[0])
    {
        return CA_ERR_BAD_ARG;
    }

    if (':' == pOptsStrPos[1])
    {        
        if ((pDatum->nScanArgvPos + 1) > pDatum->nArgc)
        {
            return CA_ERR_BAD_ARG;
        }

        *pOptCh = chOpt;        
        *pszOptArg = (char *)pDatum->pArgv[pDatum->nScanArgvPos + 1];

        pDatum->nScanArgvPos += 2;
    }
    else
    {
        *pOptCh = chOpt;
        *pszOptArg = NULL;
        pDatum->nScanArgvPos++;
    }

    return CA_ERR_SUCCESS;
}
