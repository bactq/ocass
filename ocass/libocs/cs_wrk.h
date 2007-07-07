/**
 *
 */

#ifndef _CS_WRK_H_
#define _CS_WRK_H_ 1

#include "ca_types.h"
#include "ca_cfg.h"
#include "cs_proto.h"

typedef struct _CSWrk CSWrk;

CAErrno CS_WrkStart(HMODULE hLib, CACfgDatum *pCfgDatum, 
                    CSWrk **pCSWrk);

CAErrno CS_WrkStop(CSWrk *pCSWrk);

BOOL CS_WrkIsPause(CSWrk *pCSWrk);

CAErrno CS_WrkTouchSem(CSWrk *pCSWrk, DWORD dwTouchCnt);

const CASpyDatum* CS_WrkGetSD(const CSWrk *pCSWrk);

void CS_WrkAddRawBuf(CSWrk *pCSWrk, CSProtoBuf *pProtoBuf, 
                     CSProtoType protoType);

#endif /* !defined(_CS_WRK_H_) */
