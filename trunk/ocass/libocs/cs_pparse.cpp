/**
 *
 */

#include "ca_buf.h"
#include "ca_base64.h"
#include "cs_pparse.h"

static CAErrno CS_PPGetMsgHdr(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                              char *pszMsgBuf, DWORD dwMsgBufCnt)
{
    const char *pszFind;
    const char *pszMsTxtFmtHdr = "Ms-Text-Format: ";
    const char *pszMsgBodyHdr = "ms-body=";
    const char *pszMsgStart;
    const char *pMsTxtHdr;
    const char *pMsTxtTail; 
    const char *pMsgPos; 
    DWORD dwFindLen;
    DWORD dwIns;
    char szRawMsg[CS_PP_SIP_MSG_MAX];
    int nDecodeLen;

    /* proto example
     * Ms-Text-Format: text/plain; \
     * charset=UTF-8;msgr=WAA;ms-body=ODg4IHRlc3QgODk5OQ==
     */
    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszMsTxtFmtHdr, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pMsTxtHdr = pszFind;

    dwFindLen = pPPHB->dwPHdrLen - (pMsTxtHdr - pPPHB->pPHdr);
    pszFind = CA_BMatching(pMsTxtHdr, dwFindLen, 
        CS_PP_SIP_SUB_NEWLINE, CS_PP_SIP_SUB_NEWLINE_CNT);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pMsTxtTail = pszFind;

    dwFindLen = pMsTxtTail - pMsTxtHdr;
    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        "ms-body=", FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pszMsgStart = pszFind + strlen("ms-body=");

    szRawMsg[0] = '\0';
    for (pMsgPos = pszMsgStart, dwIns = 0;; pMsgPos++, dwIns++)
    {
        if (!CA_Base64IsEncoded(pMsgPos[0]))
        {
            break;
        }

        if (dwIns >= sizeof(szRawMsg))
        {
            return CA_ERR_BAD_SEQ;
        }

        szRawMsg[dwIns] = pMsgPos[0];
    }
    szRawMsg[dwIns] = '\0';

    nDecodeLen = CA_Base64DecodeLen(szRawMsg);
    if (nDecodeLen >= (int)dwMsgBufCnt)
    {
        return CA_ERR_BAD_SEQ;
    }

    CA_Base64Decode(pszMsgBuf, szRawMsg);
    return CA_ERR_SUCCESS;
}

static CAErrno CS_PPGetMsgBody(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                               char *pszMsgBuf, DWORD dwMsgBufCnt)
{
    const char *pszFind;
    const char *pszContLen = "Content-Length: ";
    const char *pContLenHdr;
    const char *pPos;
    DWORD dwContentLen;
    DWORD dwRealContentLen;
    DWORD dwIns;
    char szContentLen[128];

    /* proto example
     * Content-Length: 4
     */
    if (NULL == pPPHB->pPBody || 0 >= pPPHB->dwPBodyLen)
    {
        return CA_ERR_BAD_SEQ;
    }

    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszContLen, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pContLenHdr = pszFind + strlen(pszContLen);

    szContentLen[0] = '\0';
    for (pPos = pContLenHdr, dwIns = 0;; dwIns++, pPos++)
    {
        if (!isdigit(pPos[0]))
        {
            break;
        }
        if (dwIns >= sizeof(szContentLen))
        {
            return CA_ERR_BAD_SEQ;
        }

        szContentLen[dwIns] = pPos[0];
    }
    szContentLen[dwIns] = '\0';
    try
    {
        dwContentLen = atol(szContentLen);
    }
    catch (...)
    {
        return CA_ERR_BAD_SEQ;
    }

    pszMsgBuf[0] = '\0';
    dwRealContentLen = (dwContentLen > pPPHB->dwPBodyLen ? 
        pPPHB->dwPBodyLen : dwContentLen);
    for (pPos = pPPHB->pPBody, dwIns = 0;; pPos++, dwIns++)
    {
        if (dwIns >= dwMsgBufCnt || dwIns >= dwRealContentLen)
        {
            break;
        }

        pszMsgBuf[dwIns] = pPos[0];
    }
    pszMsgBuf[dwIns] = '\0';
    return CA_ERR_SUCCESS;
}

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

CAErrno CS_PPBodySplit(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB)
{
    const char *pFind;

    pFind = CA_BMatching(pPSlot->protoData, pPSlot->dwPDataLen, 
        CS_PP_SIP_HDR_END, CS_PP_SIP_HDR_END_CNT);
    if (NULL == pFind)
    {
        pPPHB->pPHdr = pPSlot->protoData;
        pPPHB->dwPHdrLen = pPSlot->dwPDataLen;

        pPPHB->pPBody = NULL;
        pPPHB->dwPBodyLen = 0;
        return CA_ERR_SUCCESS;
    }

    pPPHB->pPHdr = pPSlot->protoData;
    pPPHB->dwPHdrLen = pFind - pPPHB->pPHdr;
    pFind += CS_PP_SIP_HDR_END_CNT;
    pPPHB->pPBody = pFind;
    pPPHB->dwPBodyLen = pPSlot->dwPDataLen - pPPHB->dwPHdrLen - 
        CS_PP_SIP_HDR_END_CNT;
    if (0 >= pPPHB->dwPBodyLen || 0 >= pPPHB->dwPHdrLen)
    {
        return CA_ERR_BAD_SEQ;
    }

    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetCallId(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                       char *pszCallIdBuf, DWORD dwCallIdBufCnt)
{
    const char *pszCallIdHdr = "CALL-ID: ";
    const char *pszFind;
    const char *pszPos;
    DWORD dwCallIdPos;

    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszCallIdHdr, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }

    pszCallIdBuf[0] = '\0';
    pszPos = pszFind + strlen(pszCallIdHdr);
    for (dwCallIdPos = 0;; pszPos++, dwCallIdPos++)
    {
        if (dwCallIdPos >= dwCallIdBufCnt)
        {
            break;
        }

        if (!isalpha(pszPos[0]) && !isdigit(pszPos[0]))
        {
            break;
        }

        pszCallIdBuf[dwCallIdPos] = pszPos[0];
    }
    pszCallIdBuf[dwCallIdPos] = '\0';
    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetCSeq(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     DWORD *pdwCSeq)
{
    const char *pszFind;
    const char *pszPos;
    const char *pszCSeq = "CSEQ: ";
    DWORD dwInsPos;
    CHAR szNum[128];

    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszCSeq, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }

    szNum[0] = '\0';
    pszPos = pszFind + strlen(pszCSeq);
    for (dwInsPos = 0;; pszPos++, dwInsPos++)
    {
        if (dwInsPos >= sizeof(szNum))
        {
            break;
        }

        if (!isdigit(pszPos[0]))
        {
            break;
        }

        szNum[dwInsPos] = pszPos[0];
    }
    szNum[dwInsPos] = '\0';

    try
    {
        *pdwCSeq = (DWORD)atol(szNum);
    }
    catch (...)
    {
        return CA_ERR_BAD_SEQ;
    }

    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetFrom(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                     char *pszFromBuf, DWORD dwFromBufCnt)
{
    const char *pszPos;
    const char *pszFind;
    const char *pszFrom = "From: ";
    const char *pFromHdr;
    const char *pFromTail;
    const char *pNameHdr;
    DWORD dwInsPos;
    DWORD dwFindLen;

    /* proto example
     * From: "L - name1"<sip:webmaster(lexx.51.net)@msn.com>;tag=571be9611
     */
    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszFrom, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pFromHdr = pszFind;

    dwFindLen = pPPHB->dwPHdrLen - (pFromHdr - pPPHB->pPHdr);
    pszFind = CA_BMatching(pFromHdr, dwFindLen, 
        CS_PP_SIP_SUB_NEWLINE, CS_PP_SIP_SUB_NEWLINE_CNT);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pFromTail = pszFind;

    dwFindLen = pFromTail - pFromHdr;
    pszFind = CA_BMatchingAlpha(pFromHdr, dwFindLen, 
                "<sip:", FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pNameHdr = pszFind + strlen("<sip:");

    pszPos = pNameHdr;
    pszFromBuf[0] = '\0';
    for (dwInsPos = 0;; pszPos++, dwInsPos++)
    {
        if (dwInsPos >= dwFromBufCnt)
        {
            break;
        }

        if ('>' == pszPos[0])
        {
            break;
        }

        pszFromBuf[dwInsPos] = pszPos[0];
    }
    pszFromBuf[dwInsPos] = '\0';

    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetTo(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                   char *pszToBuf, DWORD dwToBufCnt)
{
    const char *pszPos;
    const char *pszFind;
    const char *pszTo = "To: ";
    const char *pToHdr;
    const char *pToTail;
    const char *pNameHdr;
    DWORD dwInsPos;
    DWORD dwFindLen;

    /* proto example
     * To: "test dd"<sip:xiongjia_le@sonic.com>;epid=8dcae8e001
     */
    pszFind = CA_BMatchingAlpha(pPPHB->pPHdr, pPPHB->dwPHdrLen, 
        pszTo, FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pToHdr = pszFind;

    dwFindLen = pPPHB->dwPHdrLen - (pToHdr - pPPHB->pPHdr);
    pszFind = CA_BMatching(pToHdr, dwFindLen, 
        CS_PP_SIP_SUB_NEWLINE, CS_PP_SIP_SUB_NEWLINE_CNT);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pToTail = pszFind;

    dwFindLen = pToTail - pToHdr;
    pszFind = CA_BMatchingAlpha(pToHdr, dwFindLen, 
                "<sip:", FALSE);
    if (NULL == pszFind)
    {
        return CA_ERR_BAD_SEQ;
    }
    pNameHdr = pszFind + strlen("<sip:");

    pszPos = pNameHdr;
    pszToBuf[0] = '\0';
    for (dwInsPos = 0;; pszPos++, dwInsPos++)
    {
        if (dwInsPos >= dwToBufCnt)
        {
            break;
        }

        if ('>' == pszPos[0])
        {
            break;
        }

        pszToBuf[dwInsPos] = pszPos[0];
    }
    pszToBuf[dwInsPos] = '\0';

    return CA_ERR_SUCCESS;
}

CAErrno CS_PPGetMsg(CSProtoRawSlot *pPSlot, CSPProtoHB *pPPHB, 
                    char *pszMsgBuf, DWORD dwMsgBufCnt)
{
    if (CS_PROTO_INVITE == pPSlot->protoType)
    {
        return CS_PPGetMsgHdr(pPSlot, pPPHB, pszMsgBuf, dwMsgBufCnt);
    }
    else
    {
        return CS_PPGetMsgBody(pPSlot, pPPHB, pszMsgBuf, dwMsgBufCnt);
    }
}
