/*
 * Copyright (C) 2007 LeXiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 * This file is part of ocass
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _CS_PROTO_H_
#define _CS_PROTO_H_ 1

#include "ca_types.h"
#include "ca_chr.h"

typedef enum
{
    CS_PROTO_OTHER = -1,
    CS_PROTO_MESSAGE = 0,
    CS_PROTO_INVITE, 
    CS_PROTO_OK, 
} CSProtoType;

typedef struct _CSProtoBuf
{
    SOCKET netFd;
    const char *pBuf;
    int nBufLen;
    BOOL bIsRcv;
    int nApiSlot;
} CSProtoBuf;

#define CS_PROTO_RAW_MAX_BUF        (1024 * 4)

typedef struct _CSProtoRawSlot
{
    CSProtoType protoType;
    time_t tmAppend;
    BOOL bIsRcv;
    DWORD dwPDataLen;
    char protoData[CS_PROTO_RAW_MAX_BUF];

    struct _CSProtoRawSlot *pNext;
} CSProtoRawSlot;

typedef struct _CSPProto
{
    BOOL bIsRcv;
    CSProtoType protoType;
    time_t tmAppend;

    char szCall_ID[128];
    DWORD dwCSeq;

    char szFrom[128];
    char szTo[128];

    char szMsg[1024 * 3];
} CSPProto;

#define CS_PROTO_CACHE_ITEM_MAX      (888)

typedef struct _CSProtoCache
{
    UINT nAllocCnt;

    UINT nProcessCnt;
    CSProtoRawSlot *pProcessHdr;
    CSProtoRawSlot *pProcessTail;

    UINT nFreeCnt;
    CSProtoRawSlot *pFreeHdr;

    CACHRec *pCHR;
} CSProtoCache;

CAErrno CS_ProtoCacheStartup(CSProtoCache *pCache);
void CS_ProtoCacheCleanup(CSProtoCache *pCache);

void CS_ProtoAddBufItem(CSProtoBuf *pProtoBuf);

void CS_ProtoCacheRawAdd(CSProtoCache *pCache, CSProtoBuf *pProtoBuf, 
                         CSProtoType protoType);

CAErrno CS_ProtoCacheProcess(CSProtoCache *pCache, BOOL bIsPauseMon);

CAErrno CS_ProtoCacheItemProcess(CSProtoCache *pCache, 
                                 CSProtoRawSlot *pPSlot);

CAErrno CS_ProtoRawDup(CSProtoBuf *pPBuf, CSProtoType protoType, 
                       CSProtoRawSlot *pProtoSlot);

CAErrno CS_ProtoItemProcess(CSProtoCache *pCache, 
                            CSPProto *pPI);

#endif /* !defined(_CS_PROTO_H_) */
