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

#include "ca_xf.h"
#include "ca_str.h"
#include "ca_mm.h"
#include "ca_inner.h"

#import <msxml4.dll> named_guids
using namespace MSXML2;

struct _CAXF
{
    TCHAR szXFName[MAX_PATH];

    IXMLDOMDocument2Ptr xfDocPtr;
    IXMLDOMElementPtr rootNodePtr;
};

static CAErrno CA_XmlFLoad(const TCHAR *pszFName, CAXF *pXF)
{
    VARIANT_BOOL varBoolRet;
    CAErrno funcErr = CA_ERR_SUCCESS;
    _bstr_t strVal = TEXT("");
    HRESULT hResult;
    int nResult;

    pXF->szXFName[0] = '\0';
    pXF->xfDocPtr = NULL;
    pXF->rootNodePtr = NULL;

    nResult = CA_SNPrintf(pXF->szXFName, 
        sizeof(pXF->szXFName) / sizeof(pXF->szXFName[0]),
        TEXT("%s"), pszFName);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    try
    {
        hResult = pXF->xfDocPtr.CreateInstance(CLSID_DOMDocument);
        if (FAILED(hResult))
        {
            pXF->xfDocPtr = NULL;
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
    }
    catch (...)
    {
        pXF->xfDocPtr = NULL;
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    try
    {
        varBoolRet = pXF->xfDocPtr->load(pszFName);
        if (VARIANT_TRUE != varBoolRet)
        {
            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }
    }
    catch (...)
    {
        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

    try
    {
        pXF->rootNodePtr = pXF->xfDocPtr->documentElement;
        if (NULL == pXF->rootNodePtr)
        {
            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }

        strVal = pXF->rootNodePtr->GetnodeName();
        nResult = lstrcmpi(strVal, TEXT("MsgLog"));
        if (0 != nResult)
        {
            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }
    }
    catch (...)
    {
        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pXF->rootNodePtr)
        {
            pXF->rootNodePtr.Release();
            pXF->rootNodePtr = NULL;
        }
        if (NULL != pXF->xfDocPtr)
        {
            pXF->xfDocPtr.Release();
            pXF->xfDocPtr = NULL;
        }

        pXF->szXFName[0] = '\0';
    }
    return funcErr;
}

static CAErrno CA_XmlFCreate(const TCHAR *pszFName, CAXF *pXF)
{
    IXMLDOMProcessingInstructionPtr xmlPIPtr = NULL;
    HRESULT hResult;
    CAErrno funcErr = CA_ERR_SUCCESS;
    int nResult;

    pXF->szXFName[0] = '\0';
    pXF->xfDocPtr = NULL;
    pXF->rootNodePtr = NULL;

    nResult = CA_SNPrintf(pXF->szXFName, 
        sizeof(pXF->szXFName) / sizeof(pXF->szXFName[0]),
        TEXT("%s"), pszFName);
    if (0 >= nResult)
    {
        return CA_ERR_FNAME_TOO_LONG;
    }

    try
    {
        hResult = pXF->xfDocPtr.CreateInstance(CLSID_DOMDocument);
        if (FAILED(hResult))
        {
            pXF->xfDocPtr = NULL;
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
    }
    catch (...)
    {
        pXF->xfDocPtr = NULL;
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* set xml head */
    try
    {
        xmlPIPtr = pXF->xfDocPtr->createProcessingInstruction(TEXT("xml"),
            TEXT("version=\"1.0\" encoding=\"UTF-8\""));
        if (NULL == xmlPIPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        pXF->xfDocPtr->appendChild(xmlPIPtr);

        xmlPIPtr = 
            pXF->xfDocPtr->createProcessingInstruction(TEXT("xml-stylesheet"),
            CA_XF_XSL_HDR);
        if (NULL == xmlPIPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        pXF->xfDocPtr->appendChild(xmlPIPtr);
    }
    catch (...)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* add root node */
    try
    {
        pXF->rootNodePtr = pXF->xfDocPtr->createElement(TEXT("MsgLog"));
        if (NULL == pXF->rootNodePtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        pXF->xfDocPtr->appendChild(pXF->rootNodePtr);
    }
    catch (...)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* save to disk */
    try
    {
        hResult = pXF->xfDocPtr->save(pXF->szXFName);
        if (FAILED(hResult))
        {
            funcErr = CA_ERR_FWRITE;
            goto EXIT;
        }
    }
    catch (...)
    {
        funcErr = CA_ERR_FWRITE;
        goto EXIT;
    }

EXIT:
    if (NULL != xmlPIPtr)
    {
        xmlPIPtr.Release();
    }

    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pXF->rootNodePtr)
        {
            pXF->rootNodePtr.Release();
            pXF->rootNodePtr = NULL;
        }
        if (NULL != pXF->xfDocPtr)
        {
            pXF->xfDocPtr.Release();
            pXF->xfDocPtr = NULL;
        }

        pXF->szXFName[0] = '\0';
    }
    return funcErr;
}

static CAErrno CA_XmlSetMsgNodeAttr(IXMLDOMElementPtr msgElementPtr,
                                    CAXFNode *pAttr)
{
    IXMLDOMElementPtr subNodePtr = NULL;
    IXMLDOMTextPtr nodeTxtPtr = NULL;
    _variant_t varTm(NULL == pAttr->pszTm ? TEXT("") : pAttr->pszTm);
    _variant_t varCSeq((long)pAttr->dwCSeq);
    CAErrno funcErr = CA_ERR_SUCCESS;
    HRESULT hResult;

    /* Attr example
     *
     * "<MsgItem TimeStamp=\"%s\"> \n"
     * "   <MsgCallId CSeq=\"%u\">%s</MsgCallId> \n"
     * "   <MsgFrom>%s</MsgFrom> \n"
     * "   <MsgTo>%s</MsgTo> \n"
     * "   <MsgData>%s</MsgData> \n"
     * "</MsgItem>\n"
     */
    try
    {
        /* set timestamp */
        hResult = msgElementPtr->setAttribute(TEXT("TimeStamp"), varTm);
        if (FAILED(hResult))
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        /* call id node */
        subNodePtr = 
            msgElementPtr->ownerDocument->createElement(TEXT("MsgCallId"));
        if (NULL == subNodePtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        subNodePtr->setAttribute(TEXT("CSeq"), varCSeq);
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszCallId ? TEXT("") : pAttr->pszCallId);
        if (NULL == nodeTxtPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        subNodePtr->appendChild(nodeTxtPtr);
        msgElementPtr->appendChild(subNodePtr);

        /* from node */
        subNodePtr = 
            msgElementPtr->ownerDocument->createElement(TEXT("MsgFrom"));
        if (NULL == subNodePtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszFrom ? TEXT("") : pAttr->pszFrom);
        if (NULL == nodeTxtPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        subNodePtr->appendChild(nodeTxtPtr);
        msgElementPtr->appendChild(subNodePtr);

        /* to node */
        subNodePtr = 
            msgElementPtr->ownerDocument->createElement(TEXT("MsgTo"));
        if (NULL == subNodePtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszTo ? TEXT("") : pAttr->pszTo);
        if (NULL == nodeTxtPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        subNodePtr->appendChild(nodeTxtPtr);
        msgElementPtr->appendChild(subNodePtr);

        /* msg data */
        subNodePtr = 
            msgElementPtr->ownerDocument->createElement(TEXT("MsgData"));
        if (NULL == subNodePtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszMsg ? TEXT("") : pAttr->pszMsg);
        if (NULL == nodeTxtPtr)
        {
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        subNodePtr->appendChild(nodeTxtPtr);
        msgElementPtr->appendChild(subNodePtr);
    }
    catch (...)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

EXIT:
    if (NULL != nodeTxtPtr)
    {
        nodeTxtPtr.Release();
    }

    if (NULL != subNodePtr)
    {
        subNodePtr.Release();
    }
    return funcErr;
}

CA_DECLARE(CAErrno) CA_XFOpen(const TCHAR *pszFName, BOOL bCreate, 
                              CAXF **pXF)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;
    CAXF *pNewXF = NULL;
    
    pNewXF = (CAXF*)CA_MAlloc(sizeof(CAXF));
    if (NULL == pNewXF)
    {
        return CA_ERR_NO_MEM;
    }

    pNewXF->szXFName[0] = '\0';
    pNewXF->xfDocPtr = NULL;
    pNewXF->rootNodePtr = NULL;
    if (bCreate) 
    {
        caErr = CA_XmlFCreate(pszFName, pNewXF);
    }
    else
    {
        caErr = CA_XmlFLoad(pszFName, pNewXF);
    }
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pNewXF)
        {
            CA_MFree(pNewXF);
        }
        *pXF = NULL;
    }
    else
    {
        *pXF = pNewXF;
    }

    return funcErr;
}

CA_DECLARE(void) CA_XFClose(CAXF *pXF)
{
    if (NULL == pXF)
    {
        return;
    }

    pXF->rootNodePtr.Release();
    pXF->xfDocPtr.Release();
    CA_MFree(pXF);
}

CA_DECLARE(CAErrno) CA_XFSave(CAXF *pXF)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    HRESULT hResult;
    DWORD dwFileAttrs;

    if (NULL == pXF || NULL == pXF->xfDocPtr)
    {
        return CA_ERR_BAD_ARG;
    }

    /* remove read only attr */
    dwFileAttrs = GetFileAttributes(pXF->szXFName);   
    if (((DWORD)-1 != dwFileAttrs) &&
        (dwFileAttrs & FILE_ATTRIBUTE_READONLY))
    {
        SetFileAttributes(pXF->szXFName, 
            dwFileAttrs & (~FILE_ATTRIBUTE_READONLY));
    }

    try
    {
        hResult = pXF->xfDocPtr->save(pXF->szXFName);
        if (FAILED(hResult))
        {
            funcErr = CA_ERR_FWRITE;
        }
    }
    catch (...)
    {
        funcErr = CA_ERR_FWRITE;
    }

    return funcErr;
}

CA_DECLARE(CAErrno) CA_XFAddNode(CAXF *pXF, CAXFNode *pNode)
{
    IXMLDOMElementPtr msgElementPtr = NULL;
    CAErrno funcErr = CA_ERR_SUCCESS;
    CAErrno caErr;

    if (NULL == pXF || NULL == pXF->rootNodePtr || NULL == pNode)
    {
        return CA_ERR_BAD_ARG;
    }

    try
    {
        msgElementPtr = 
            pXF->rootNodePtr->ownerDocument->createElement("MsgItem");
        if (NULL == msgElementPtr)
        {
            return CA_ERR_NO_MEM;
        }
    }
    catch (...)
    {
        return CA_ERR_NO_MEM;
    }

    caErr = CA_XmlSetMsgNodeAttr(msgElementPtr, pNode);
    if (CA_ERR_SUCCESS != caErr)
    {
        funcErr = caErr;
        goto EXIT;
    }

    /* add to root node */
    try
    {
        pXF->rootNodePtr->appendChild(msgElementPtr);
    }
    catch (...)
    {
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

EXIT:
    if (NULL != msgElementPtr)
    {
        msgElementPtr.Release();
    }
    return funcErr;
}

CA_DECLARE(const TCHAR*) CA_XFGetFName(CAXF *pXF)
{
    return pXF->szXFName;
}
