/**
 *
 */

#ifndef _CA_CHR_H_
#define _CA_CHR_H_ 1

#include "ca_types.h"

typedef struct _CA_CHART_HISTORY_REC CACHRec;

#define CA_CHR_DEFAULT_CACHE_SEC        (60 * 3)

CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR, DWORD nMaxCacheSec);

CA_DECLARE(CAErrno) CA_CHRecClose(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRCpStyleFile(CACHRec *pCHR, 
                                      const TCHAR *pszDestPath);

typedef struct _CA_CHR_ITEM
{
    time_t tmAppend;
    char *pszMaster;
    char *pszGuest;

    char *pszCallId;
    DWORD dwCSeq;

    char *pszFrom;
    char *pszTo;

    char *pszMsg;
} CACHRItem;

#define CA_CHR_MAX_RETRY        (10)

CA_DECLARE(CAErrno) CA_CHRecAppend(CACHRec *pCHR, DWORD dwRetryCnt, 
                                   CACHRItem *pCHRItem);

#endif /* !defined(_CA_CHR_H_) */
