/**
 *
 */

#ifndef _OCAC_MISC_H_
#define _OCAC_MISC_H_ 1

#include "liboca.h"
#include "libocc.h"

void OCAC_Panic(const TCHAR *pszSrc, const int nSrcLine, 
                const TCHAR *pszFmt, ...);

void OCAC_Usage(void);

void OCAC_PrintCoreState(CCWrkStateDesc *pWrkState);

#endif /* !defined(_OCAC_MISC_H_) */
