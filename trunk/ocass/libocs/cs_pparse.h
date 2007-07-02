/**
 *
 */

#ifndef _CS_PPARSE_H_
#define _CS_PPARSE_H_ 1

#include "ca_types.h"
#include "cs_proto.h"

BOOL CS_CmpProtoWithHdr(const char *pszProtoHdr, 
                        const char *pBuf, int nBufLen);

BOOL CS_IsMessageProto(const char *pBuf, int nBufLen);

BOOL CS_IsInviteProto(const char *pBuf, int nBufLen);


CAErrno  CS_RawProtoParse(CSProtoRawSlot *pRawProtoItem, CSPProto *pPProto);


#endif /* !defined(_CS_PPARSE_H_) */
