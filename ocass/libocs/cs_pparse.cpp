/**
 *
 */

#include "cs_pparse.h"

BOOL CS_CmpProtoWithHdr(const char *pszProtoHdr, 
                        const char *pBuf, int nBufLen)
{
    int nCmpLen = strlen(pszProtoHdr);
    int nResult;

    if (nBufLen <= nCmpLen)
    {
        return FALSE;
    }

    nResult = memcmp(pszProtoHdr, pBuf, nCmpLen);
    return (0 == nResult ? TRUE : FALSE);
}

BOOL CS_IsInviteProto(const char *pBuf, int nBufLen)
{
    const char *pszProtoHdr = "INVITE";
    return CS_CmpProtoWithHdr(pszProtoHdr, pBuf, nBufLen);
}

BOOL CS_IsMessageProto(const char *pBuf, int nBufLen)
{
    const char *pszProtoHdr = "MESSAGE";
    return CS_CmpProtoWithHdr(pszProtoHdr, pBuf, nBufLen);
}

CAErrno  CS_RawProtoParse(CSProtoRawSlot *pRawProtoItem, CSPProto *pPProto)
{
    memset(pPProto, 0, sizeof(CSPProto));
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPBodySplit(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetMsg(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                    char *pszMsgBuf, DWORD dwMsgBufCnt)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetCallId(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                       char *pszCallIdBuf, DWORD dwCallIdBufCnt)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetCSeq(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     DWORD *pdwCSeq)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetFrom(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     char *pszFromBuf, DWORD dwFromBufCnt)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetTo(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     char *pszFromBuf, DWORD dwFromBufCnt)
{
    /* XXX */
    return CA_ERR_SUCCESS;
}
