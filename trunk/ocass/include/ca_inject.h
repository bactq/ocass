/**
 *
 */

#ifndef _CA_INJECT_H_
#define _CA_INJECT_H_ 1

#include "ca_types.h"


CA_DECLARE(CAErrno) CA_ReplaceAPI(const TCHAR *pszModName, 
                const TCHAR *pszAPIName, PROC pNewFunc, PROC *pOldFunc);

CA_DECLARE(void) CA_ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
        PROC pfnCurrent, PROC pfnNew, BOOL fExcludeAPIHookMod);

#endif /* !defined(_CA_INJECT_H_) */
