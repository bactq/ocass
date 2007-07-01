/**
 *
 */

#include "cs_pparse.h"

BOOL CS_CmpProtoWithHdr(const char *pProtoHdr, 
                        const char *pBuf, int nBufLen)
{
    int nCmpLen = strlen(pProtoHdr);
    int nResult;

    if (nBufLen <= nCmpLen)
    {
        return FALSE;
    }

    nResult = memcmp(pProtoHdr, pBuf, nCmpLen);
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
