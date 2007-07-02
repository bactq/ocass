/**
 *
 */

#ifndef _CS_PROTO_H_
#define _CS_PROTO_H_ 1

#include "ca_types.h"

#define CS_PROTO_RAW_MAX_BUF        (1024 * 4)
#define CS_PROTO_RAW_MAX_CACHE      (1024)

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

typedef struct _CSProtoRawSlot
{
    CSProtoType protoType;
    BOOL bIsRcv;
    char szProtoData[CS_PROTO_RAW_MAX_BUF];

    struct _CSProtoSlot *pNext;
} CSProtoRawSlot;

typedef struct _CSPProto
{
    CSProtoType protoType;

    char szCall_ID[128];
    DWORD dwCSeq;

    char szFrom[128];
    char szTo[128];

    char szMsg[1024 * 3];
} CSPProto;

#define CS_PROTO_CACHE_MAX      (1000 * 3)

typedef struct _CSProtoCache
{
    UINT nAllocCnt;

    UINT nProcessCnt;
    CSProtoRawSlot *pProcessHdr;
    CSProtoRawSlot *pProcessTail;

    UINT nFreeCnt;
    CSProtoRawSlot *pFreeHdr;
} CSProtoCache;

void CS_ProtoCacheStartup(CSProtoCache *pCache);
void CS_ProtoCacheCleanup(CSProtoCache *pCache);

void CS_ProtoAddBufItem(CSProtoBuf *pProtoBuf);

void CS_ProtoCacheRawAdd(CSProtoCache *pCache, CSProtoBuf *pProtoBuf, 
                         CSProtoType protoType);

CAErrno CS_ProtoCacheProcess(CSProtoCache *pCache);

#endif /* !defined(_CS_PROTO_H_) */
