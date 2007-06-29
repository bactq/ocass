/**
 *
 */

#ifndef _CS_WRK_H_
#define _CS_WRK_H_ 1

#include "ca_types.h"
#include "ca_cfg.h"

typedef struct _CSWrk CSWrk;

CAErrno CS_WrkStart(HMODULE hLib, CACfgDatum *pCfgDatum, 
                    CSWrk **pCSWrk);

CAErrno CS_WrkStop(CSWrk *pCSWrk);

#endif /* !defined(_CS_WRK_H_) */
