/**
 *
 */

#ifndef _CS_PPARSE_H_
#define _CS_PPARSE_H_ 1

#include "ca_types.h"
#include "cs_proto.h"

#define CS_PP_SIP_MSG_MAX       (1024 * 2)

#define CS_PP_SIP_NEWLINE_CNT   (3)
#define CS_PP_SIP_NEWLINE       ("\r\r\n")

#define CS_PP_SIP_SUB_NEWLINE_CNT   (2)
#define CS_PP_SIP_SUB_NEWLINE       ("\r\n")

#define CS_PP_SIP_HDR_END_CNT   (4)
#define CS_PP_SIP_HDR_END       ("\r\n\r\n")

BOOL CS_CmpProtoWithHdr(const char *pszProtoHdr, 
                        const char *pBuf, int nBufLen);

BOOL CS_IsMessageProto(const char *pBuf, int nBufLen);

BOOL CS_IsInviteProto(const char *pBuf, int nBufLen);

typedef struct _CS_PPHB
{
    const char *pPHdr;
    DWORD dwPHdrLen;

    const char *pPBody;
    DWORD dwPBodyLen;
} CSPProtoHB;

CAErrno CS_PPBodySplit(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB);

CAErrno CS_PPGetMsg(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                    char *pszMsgBuf, DWORD dwMsgBufCnt);

CAErrno CS_PPGetCallId(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                       char *pszCallIdBuf, DWORD dwCallIdBufCnt);

CAErrno CS_PPGetCSeq(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     DWORD *pdwCSeq);

CAErrno CS_PPGetFrom(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     char *pszFromBuf, DWORD dwFromBufCnt);

CAErrno CS_PPGetTo(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                   char *pszToBuf, DWORD dwToBufCnt); 

#endif /* !defined(_CS_PPARSE_H_) */
