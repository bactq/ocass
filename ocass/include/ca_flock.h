/**
 *
 */

#ifndef _CA_FLOCK_H_
#define _CA_FLOCK_H_ 1

#include "ca_types.h"

typedef struct _CA_FLock_
{
    HANDLE hFile;
	char szLockFName[MAX_PATH];
	OVERLAPPED overLapped;
} CAFLock;

typedef enum
{
    CA_FLOCK_OPT_RD = 1, 
    CA_FLOCK_OPT_RW, 
} CAFLockOpt;

CA_DECLARE(CAErrno) CA_FLockCreate(const char *pszFileName, CAFLock *pFLock);
CA_DECLARE(CAErrno) CA_FLockDestroy(CAFLock *pFLock, BOOL bUnlinkLockFile);

CA_DECLARE(CAErrno) CA_FLockEnter(CAFLock *pFLock, CAFLockOpt lockOpt);
CA_DECLARE(CAErrno) CA_FLockLeave(CAFLock *pFLock);


#endif /* !defined(_CA_FLOCK_H_) */
