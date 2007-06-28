/**
 *
 */

#ifndef _CS_NETI_H_
#define _CS_NETI_H_ 1

#include "ca_types.h"

#define CS_NETWRK_API_COUNT        3
#define CS_NETWRK_API_SND_VER1     0
#define CS_NETWRK_API_SND          1
#define CS_NETWRK_API_WSARCV       2

typedef struct CS_NeTWRK_API_SLOT
{
    BOOL bInUsed;
    BOOL bHooked;
    TCHAR *pszModName;
    TCHAR *pszAPIName;
    PROC pOldProc;
    PROC pNewProc;
} CSNetWrkApiSlot;

typedef struct _CS_NeTWRK_API_LIST
{
    INT nSlotId;
    TCHAR *pszModName;
    TCHAR *pszAPIName;
    PROC pNewProc;
} CSNetWrkApiList;

CSNetWrkApiSlot* CS_NtWrkGetSlot(int nIndex);

CAErrno CS_NtWrkApiAttach(void);

void CS_OnNtWrkIO(SOCKET netFd, const char *pBuf, 
                  int nBufLen, int nApiSlotId);

#endif /* !defined(_CS_NETI_H_) */
