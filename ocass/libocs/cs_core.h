/**
 *
 */

#ifndef _CS_CORE_H_
#define _CS_CORE_H_ 1

#include "ca_types.h"
#include "cs_wrk.h"

const CSWrk* CS_CoreGetWrkPtr(void);
void CS_CoreOnDllLoad(HINSTANCE hInst);
void CS_CoreOnDllUnload(HINSTANCE hInst);

#endif /* !defined(_CS_CORE_H_) */
