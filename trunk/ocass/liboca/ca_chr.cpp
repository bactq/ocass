/**
 *
 */

#include "ca_chr.h"

typedef struct _CA_CHR_REC_FD_SLOT
{
    BOOL bIsUsed;
    FILE *pFile;
    TCHAR szFName[MAX_PATH];
    time_t tmOpen;
} CACHRRecFdSlot;

struct _CA_CHART_HISTORY_REC
{
    int x;
};

CA_DECLARE(CAErrno) CA_CHRecOpen(CACHRec **pCHR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecClose(CACHRec *pCHR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecUpdateCfg(CACHRec *pCHR)
{
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_CHRecAppend(CACHRec *pCHR, DWORD dwRetryCnt, 
                                   CACHRItem *pCHRItem)
{
    return CA_ERR_SUCCESS;
}
