/**
 *
 */

#include "ca_flock.h"

CA_DECLARE(CAErrno) CA_FLockCreate(const char *pszFileName, CAFLock *pFLock)
{
    CAErrno caErrFunc = CA_ERR_SUCCESS;
    HANDLE hEvent = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwWrBytes;
    BOOL bResult;
    int nResult;

    /* save the lock file name */
    nResult = _snprintf(pFLock->szLockFName, 
        sizeof(pFLock->szLockFName), "%s", pszFileName);
    if (0 >= nResult)
    {
        caErrFunc = CA_ERR_FNAME_BAD;
        goto FAILED_EXIT;
    }

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == hEvent)
    {
        caErrFunc = CA_ERR_SYS_CALL;
        goto FAILED_EXIT;
    }

    hFile = CreateFile(pszFileName, GENERIC_READ|GENERIC_WRITE, 
        FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) 
    {
        caErrFunc = CA_ERR_SYS_CALL;
        goto FAILED_EXIT;
    }

    /* write the first byte "1" to lock/unlock */
    bResult = WriteFile(hFile, "1", 1, &dwWrBytes, NULL);
    if (!bResult)
    {
        /* XXX disk over ??? */
        caErrFunc = CA_ERR_SYS_CALL;
        goto FAILED_EXIT;
    }

    pFLock->hFile = hFile;
    pFLock->overLapped.hEvent = hEvent;
    return caErrFunc;

FAILED_EXIT:
    if (NULL != hEvent)
    {
        CloseHandle(hEvent);
    }

    if (INVALID_HANDLE_VALUE != hFile)
    {
        CloseHandle(hFile);
    }

    return caErrFunc;
}

CA_DECLARE(CAErrno) CA_FLockDestroy(CAFLock *pFLock, BOOL bUnlinkLockFile)
{
    if (NULL == pFLock)
    {
        return CA_ERR_SUCCESS;
    }

    pFLock->szLockFName[0] = '\0';
    if (INVALID_HANDLE_VALUE != pFLock->hFile)
    {
        CloseHandle(pFLock->hFile);
        pFLock->hFile = INVALID_HANDLE_VALUE;
    }

    if (NULL != pFLock->overLapped.hEvent)
    {
        CloseHandle(pFLock->overLapped.hEvent);
        pFLock->overLapped.hEvent = NULL;
    }

    /* XXX delete the lock file */
    return CA_ERR_SUCCESS;
}

CA_DECLARE(CAErrno) CA_FLockEnter(CAFLock *pFLock, CAFLockOpt lockOpt)
{
    DWORD nFlags = (lockOpt == CA_FLOCK_OPT_RW ? LOCKFILE_EXCLUSIVE_LOCK : 0);
    BOOL bResult;

    bResult = LockFileEx(pFLock->hFile, nFlags, 0, 1, 0, &pFLock->overLapped);
    return (bResult ? CA_ERR_SUCCESS : CA_ERR_SYS_CALL);
}

CA_DECLARE(CAErrno) CA_FLockLeave(CAFLock *pFLock)
{
    BOOL bResult;

    bResult = UnlockFileEx(pFLock->hFile, 0, 1, 0, &pFLock->overLapped);
    return (bResult ? CA_ERR_SUCCESS : CA_ERR_SYS_CALL);
}
