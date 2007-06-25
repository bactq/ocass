/**
 *
 */

#ifndef _OCAC_PROC_H_
#define _OCAC_PROC_H_ 1

#include "liboca.h"

typedef enum
{    
    OCAC_PROC_TYPE_WORKER = 0,
    OCAC_PROC_TYPE_KILLER,
    OCAC_PROC_TYPE_USAGE,
} OCACProcType;

typedef struct _OCAC_PROC_
{
    OCACProcType procType;
    TCHAR szProcWrkPath[MAX_PATH];
    TCHAR szProcCfgFName[MAX_PATH];
} OCACProc;

CAErrno CAC_PStartup(int nArgc, char **pArgv, OCACProc *pProc);
CAErrno CAC_PCleanup(OCACProc *pProc);

#endif /* !defined(_OCAC_PROC_H_) */
