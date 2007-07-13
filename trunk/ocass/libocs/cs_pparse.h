/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
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
