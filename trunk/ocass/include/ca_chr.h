/**
 *
 */

#ifndef _CA_CHR_H_
#define _CA_CHR_H_ 1

#include "ca_types.h"

typedef struct _CA_CHART_HISTORY_REC CACHRec;


CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR);

CA_DECLARE(CAErrno) CA_CHRecClose(CACHRec *pCHR);

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR);

typedef struct _CA_CHR_ITEM
{
    time_t tmAppend;
    char *pszMaster;
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
