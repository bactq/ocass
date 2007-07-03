/**
 *
 */

#include "cs_proto.h"
#include "ca_sr.h"
#include "ca_mm.h"
#include "ca_buf.h"
#include "ca_chr.h"
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

CAErrno CS_ProtoRawDup(CSProtoBuf *pPBuf, CSProtoType protoType, 
                       CSProtoRawSlot *pProtoSlot)
{
    int nDestBLen;

    pProtoSlot->pNext = NULL;
    pProtoSlot->bIsRcv = pPBuf->bIsRcv;
    pProtoSlot->protoType = protoType;

    nDestBLen =  sizeof(pProtoSlot->protoData);
    CA_BTruncateNCpy(pPBuf->pBuf, pPBuf->nBufLen, 
        pProtoSlot->protoData, &nDestBLen);
    pProtoSlot->dwPDataLen = nDestBLen;
    return CA_ERR_SUCCESS;
}

CAErrno CS_ProtoCacheStartup(CSProtoCache *pCache)
{
    CAErrno caErr;

    memset(pCache, 0, sizeof(CSProtoCache));
    caErr = CA_CHRecOpen(&pCache->pCHR);
    if (CA_ERR_SUCCESS == caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_ERR, 
            TEXT("Open chr handle failed. error %u"), 
            caErr);
        return caErr;
    }

    return CA_ERR_SUCCESS;
}

void CS_ProtoCacheCleanup(CSProtoCache *pCache)
{
    CSProtoRawSlot *pPSlot;
    if (NULL == pCache)
    {
        return;
    }

    for (pPSlot = pCache->pFreeHdr; NULL != pCache->pFreeHdr;)
    {
        pCache->pFreeHdr = pPSlot->pNext;
        CA_MFree(pPSlot);
    }

    for (pPSlot = pCache->pProcessHdr; NULL != pCache->pProcessHdr;)
    {
        pCache->pProcessHdr = pPSlot->pNext;
        CA_MFree(pPSlot);
    }

    if (NULL != pCache->pCHR)
    {
        CA_CHRecClose(pCache->pCHR);
    }

    CA_MFree(pCache);
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
    CSProtoRawSlot *pNewSlot;
    CAErrno caErr;
    BOOL bIsNewAlloc = FALSE;

    if (CS_PROTO_CACHE_ITEM_MAX <= pCache->nAllocCnt &&
        0 == pCache->nFreeCnt)
    {
        /* no free token */
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("proto cache over, all alloc %u, free %u, process %u"), 
            pCache->nAllocCnt, pCache->nFreeCnt, pCache->nProcessCnt);
        return;
    }
    
    if (NULL == pCache->pFreeHdr)
    {
        /* alloc new slot */
        pNewSlot = (CSProtoRawSlot *)CA_MAlloc(sizeof(CSProtoRawSlot));
        if (NULL == pNewSlot)
        {
            CS_Log(CA_SRC_MARK, CS_LOG_ERR, 
                TEXT("proto cache alloc failed. "
                "all alloc %u, free %u, process %u"), 
                pCache->nAllocCnt, pCache->nFreeCnt, pCache->nProcessCnt);
            return;
        }

        bIsNewAlloc = TRUE;
        pCache->nAllocCnt++;
    }
    else
    {
        pNewSlot = pCache->pFreeHdr;
        pCache->pFreeHdr = pNewSlot->pNext;
        pCache->nFreeCnt--;
        pCache->nFreeCnt = (0 >= pCache->nFreeCnt ? 0 : pCache->nFreeCnt);        
    }

    caErr = CS_ProtoRawDup(pProtoBuf, protoType, pNewSlot);
    if (CA_ERR_SUCCESS != caErr)
    {
        pNewSlot->pNext = pCache->pFreeHdr;
        pCache->pFreeHdr = pNewSlot;
        pCache->nFreeCnt++;
        return;
    }

    pNewSlot->pNext = NULL;
    if (NULL == pCache->pProcessTail)
    {
        pCache->pProcessTail = pNewSlot;
    }
    else
    {
        pCache->pProcessTail->pNext = pNewSlot;
        pCache->pProcessTail = pNewSlot;
    }

    if (NULL == pCache->pProcessHdr)
    {
        pCache->pProcessHdr = pCache->pProcessTail;
    }

    pCache->nProcessCnt++;
}

CAErrno CS_ProtoCacheProcess(CSProtoCache *pCache)
{
    CSProtoRawSlot *pCacheItem;
    CAErrno caErr;

    if (NULL == pCache->pProcessHdr)
    {
        return CA_ERR_SUCCESS;
    }

    pCache->nProcessCnt--;
    pCache->nProcessCnt = (0 >= pCache->nProcessCnt ? 
        0 : pCache->nProcessCnt);
    pCacheItem = pCache->pProcessHdr;
    pCache->pProcessHdr = pCacheItem->pNext;
    if (NULL == pCache->pProcessHdr)
    {
        pCache->pProcessTail = NULL;
    }

    caErr = CS_ProtoCacheItemProcess(pCache, pCacheItem);
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("process cache item failed. error %u"), 
            caErr);
    }

    pCacheItem->pNext = pCache->pFreeHdr;
    pCache->pFreeHdr = pCacheItem;
    pCache->nFreeCnt++;
    return CA_ERR_SUCCESS;
}

CAErrno CS_ProtoCacheItemProcess(CSProtoCache *pCache, 
                                 CSProtoRawSlot *pPSlot)
{
    CSPProtoHB parseProtoHB;
    CSPProto csPProto;
    CAErrno caErr;

    caErr = CS_PPBodySplit(pPSlot, &parseProtoHB);
    if (CA_ERR_SUCCESS == caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto hdr and body failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    /* init proto type */
    csPProto.bIsRcv = pPSlot->bIsRcv;
    csPProto.protoType = pPSlot->protoType;
    caErr = CS_PPGetMsg(pPSlot, &parseProtoHB, 
        csPProto.szMsg, sizeof(csPProto.szMsg));
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto msg field failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    caErr = CS_PPGetCallId(pPSlot, &parseProtoHB, 
        csPProto.szCall_ID, sizeof(csPProto.szCall_ID));
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto call id field failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    caErr = CS_PPGetCSeq(pPSlot, &parseProtoHB, &csPProto.dwCSeq);
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto CSeq field failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    caErr = CS_PPGetFrom(pPSlot, &parseProtoHB, 
        csPProto.szFrom, sizeof(csPProto.szFrom));
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto from field failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    caErr = CS_PPGetTo(pPSlot, &parseProtoHB, 
        csPProto.szTo, sizeof(csPProto.szTo));
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("parse proto to field failed, "
            "error %u, proto type %u."), 
            caErr, pPSlot->protoType);
        return caErr;
    }

    return CS_ProtoItemProcess(pCache, &csPProto);
}

CAErrno CS_ProtoItemProcess(CSProtoCache *pCache, 
                            CSPProto *pPI)
{
    CACHRItem chrItem;
    CAErrno caErr;

    if (NULL == pCache || NULL == pCache->pCHR || NULL == pPI)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("Can't writ chart history, bad args"));
        return CA_ERR_BAD_SEQ;
    }

    chrItem.pszFrom = pPI->szFrom;
    chrItem.pszTo = pPI->szTo;
    chrItem.pszMsg = pPI->szMsg;

    chrItem.tmAppend = time(NULL);
    chrItem.dwCSeq = pPI->dwCSeq;
    chrItem.pszCallId = pPI->szCall_ID;


    chrItem.pszMaster   = pPI->bIsRcv ? chrItem.pszTo : chrItem.pszFrom;
    chrItem.pszGuest    = pPI->bIsRcv ? chrItem.pszFrom : chrItem.pszTo;

    caErr = CA_CHRecAppend(pCache->pCHR, 20, &chrItem);
    if (CA_ERR_SUCCESS != caErr)
    {
        CS_Log(CA_SRC_MARK, CS_LOG_WARN, 
            TEXT("Append chart hitory failed. err %u"), caErr);
    }

    return caErr;
}
