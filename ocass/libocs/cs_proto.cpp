/**
 *
 */

#include "cs_proto.h"
#include "ca_sr.h"
#include "cs_wrk.h"
#include "cs_core.h"
#include "cs_log.h"
#include "cs_pparse.h"

static BOOL CS_ProtoNeedFilter(CSProtoBuf *pProtoBuf, 
                               CSProtoType *pProtoType)
{
    BOOL bResult;

    *pProtoType = CS_PROTO_OTHER;
    bResult = CS_IsMessageProto(pProtoBuf->pBuf, pProtoBuf->nBufLen);
    if (bResult)
    {
        *pProtoType = CS_PROTO_MESSAGE;
        return FALSE;
    }

    bResult = CS_IsInviteProto(pProtoBuf->pBuf, pProtoBuf->nBufLen);
    if (bResult)
    {
        *pProtoType = CS_PROTO_INVITE;
        return FALSE;
    }

    return TRUE;
}

void CS_ProtoCacheStartup(CSProtoCache *pCache)
{
    memset(pCache, 0, sizeof(CSProtoCache));
}

void CS_ProtoCacheCleanup(CSProtoCache *pCache)
{
    /* free all mem */
}

void CS_ProtoAddBufItem(CSProtoBuf *pProtoBuf)
{
    const CSWrk *pCSWrk;
    CSProtoType protoType;
    BOOL bResult;

    pCSWrk = CS_CoreGetWrkPtr();
    if (NULL == pCSWrk)
    {
        return;
    }

    /* filter proto */
    bResult = CS_ProtoNeedFilter(pProtoBuf, &protoType);
    CS_LogNtDump(CA_SRC_MARK, pProtoBuf->nApiSlot, bResult, 
        pProtoBuf->pBuf, pProtoBuf->nBufLen);
    if (bResult)
    {
        return;
    }

    /* add to cache proto */
    CS_WrkAddRawBuf((CSWrk *)pCSWrk, pProtoBuf, protoType);
}

void CS_ProtoCacheRawAdd(CSProtoCache *pCache, CSProtoBuf *pProtoBuf, 
                         CSProtoType protoType)
{
    /* add to slots */
}

CAErrno CS_ProtoCacheProcess(CSProtoCache *pCache)
{
    /* parse from slots */
    return CA_ERR_SUCCESS;
}
