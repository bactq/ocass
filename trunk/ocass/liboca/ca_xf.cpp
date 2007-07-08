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

#include "liboca.h"
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
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Load xml file failed. file name (%s) is too long"), 
            pszFName);
        return CA_ERR_FNAME_TOO_LONG;
    }

    try
    {
        hResult = pXF->xfDocPtr.CreateInstance(CLSID_DOMDocument);
        if (FAILED(hResult))
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file failed. Create MSXML instance failed."));
                
            pXF->xfDocPtr = NULL;
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Load xml file failed. Create MSXML instance failed."));

        pXF->xfDocPtr = NULL;
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    try
    {
        varBoolRet = pXF->xfDocPtr->load(pszFName);
        if (VARIANT_TRUE != varBoolRet)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file from (%s) failed"), pszFName);

            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file from (%s) failed"), pszFName);

        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

    try
    {
        pXF->rootNodePtr = pXF->xfDocPtr->documentElement;
        if (NULL == pXF->rootNodePtr)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file from (%s) failed, can't parse root node"), 
                pszFName);

            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }

        strVal = pXF->rootNodePtr->GetnodeName();
        nResult = lstrcmpi(strVal, TEXT("MsgLog"));
        if (0 != nResult)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file from (%s) failed, root node is bad"), 
                pszFName);

            funcErr = CA_ERR_FOPEN;
            goto EXIT;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Load xml file from (%s) failed, root node is bad"), 
            pszFName);

        funcErr = CA_ERR_FOPEN;
        goto EXIT;
    }

EXIT:
    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pXF->rootNodePtr)
        {
            try
            {
                pXF->rootNodePtr.Release();
            }
            catch (...)
            {
            }

            pXF->rootNodePtr = NULL;
        }
        if (NULL != pXF->xfDocPtr)
        {
            try
            {
                pXF->xfDocPtr.Release();
            }
            catch (...)
            {
            }

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
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xml file (%s) failed. file name too long"),
            pszFName);
        return CA_ERR_FNAME_TOO_LONG;
    }

    try
    {
        hResult = pXF->xfDocPtr.CreateInstance(CLSID_DOMDocument);
        if (FAILED(hResult))
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Load xml file failed. Create MSXML instance failed."));

            pXF->xfDocPtr = NULL;
            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Load xml file failed. Create MSXML instance failed."));
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
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create xml failed. can't set xml hdr"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        pXF->xfDocPtr->appendChild(xmlPIPtr);

        xmlPIPtr = 
            pXF->xfDocPtr->createProcessingInstruction(TEXT("xml-stylesheet"),
            CA_XF_XSL_HDR);
        if (NULL == xmlPIPtr)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create xml failed. can't set xml hdr"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        pXF->xfDocPtr->appendChild(xmlPIPtr);
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xml failed. can't set xml hdr"));

        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* add root node */
    try
    {
        pXF->rootNodePtr = pXF->xfDocPtr->createElement(TEXT("MsgLog"));
        if (NULL == pXF->rootNodePtr)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create xml failed. can't create root node"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        pXF->xfDocPtr->appendChild(pXF->rootNodePtr);
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xml failed. can't create root node"));

        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

    /* save to disk */
    try
    {
        hResult = pXF->xfDocPtr->save(pXF->szXFName);
        if (FAILED(hResult))
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create xml failed. can't save file to %s"), 
                pXF->szXFName);

            funcErr = CA_ERR_FWRITE;
            goto EXIT;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create xml failed. can't save file to %s"), 
            pXF->szXFName);

        funcErr = CA_ERR_FWRITE;
        goto EXIT;
    }

EXIT:
    if (NULL != xmlPIPtr)
    {
        try
        {
            xmlPIPtr.Release();
        }
        catch (...)
        {
        }
    }

    if (CA_ERR_SUCCESS != funcErr)
    {
        if (NULL != pXF->rootNodePtr)
        {
            try
            {
                pXF->rootNodePtr.Release();
            }
            catch (...)
            {
            }

            pXF->rootNodePtr = NULL;
        }
        if (NULL != pXF->xfDocPtr)
        {
            try
            {
                pXF->xfDocPtr.Release();
            }
            catch (...)
            {
            }

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
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Set timestamp to msg node failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        /* call id node */
        subNodePtr = 
            msgElementPtr->ownerDocument->createElement(TEXT("MsgCallId"));
        if (NULL == subNodePtr)
        {
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create callid sub node failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }

        subNodePtr->setAttribute(TEXT("CSeq"), varCSeq);        
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszCallId ? TEXT("") : pAttr->pszCallId);
        if (NULL == nodeTxtPtr)
        {
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Set callid text failed"));

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
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Create from node failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszFrom ? TEXT("") : pAttr->pszFrom);
        if (NULL == nodeTxtPtr)
        {
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("set from text failed"));
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
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("create to node failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszTo ? TEXT("") : pAttr->pszTo);
        if (NULL == nodeTxtPtr)
        {
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("set to text failed"));

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
             CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("create MsgData node failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        nodeTxtPtr = subNodePtr->ownerDocument->createTextNode(
            NULL == pAttr->pszMsg ? TEXT("") : pAttr->pszMsg);
        if (NULL == nodeTxtPtr)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("set MsgData text failed"));

            funcErr = CA_ERR_NO_MEM;
            goto EXIT;
        }
        subNodePtr->appendChild(nodeTxtPtr);
        msgElementPtr->appendChild(subNodePtr);
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Create Msg item node failed"));
        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

EXIT:
    if (NULL != nodeTxtPtr)
    {
        try
        {
            nodeTxtPtr.Release();
        }
        catch (...)
        {
        }
    }

    if (NULL != subNodePtr)
    {
        try
        {
            subNodePtr.Release();
        }
        catch (...)
        {
        }
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
         CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Open xml file (%s) failed. No memory"), 
            pszFName);
        return CA_ERR_NO_MEM;
    }

    memset(pNewXF, 0, sizeof(CAXF));

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
         CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Open xml file (%s) failed. Err %u"), 
            pszFName, caErr);

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

    try
    {
        pXF->rootNodePtr.Release();
    }
    catch (...)
    {
    }

    try
    {
        pXF->xfDocPtr.Release();
    }
    catch (...)
    {
    }
    CA_MFree(pXF);
}

CA_DECLARE(CAErrno) CA_XFSave(CAXF *pXF)
{
    CAErrno funcErr = CA_ERR_SUCCESS;
    HRESULT hResult;
    DWORD dwFileAttrs;

    if (NULL == pXF || NULL == pXF->xfDocPtr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Save xml failed. Bad args"));
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
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Save xml to (%s) failed."), 
                pXF->szXFName);

            funcErr = CA_ERR_FWRITE;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Save xml to (%s) failed."), 
            pXF->szXFName);

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
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Add xml node failed. bad args"));
        return CA_ERR_BAD_ARG;
    }

    try
    {
        msgElementPtr = 
            pXF->rootNodePtr->ownerDocument->createElement("MsgItem");
        if (NULL == msgElementPtr)
        {
            CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
                TEXT("Add xml node failed. Create element failed"));
            return CA_ERR_NO_MEM;
        }
    }
    catch (...)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Add xml node failed. Create element failed"));
        return CA_ERR_NO_MEM;
    }

    caErr = CA_XmlSetMsgNodeAttr(msgElementPtr, pNode);
    if (CA_ERR_SUCCESS != caErr)
    {
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Set data to xml element failed. err %u"), 
            caErr);

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
        CA_RTLog(CA_SRC_MARK, CA_RTLOG_ERR, 
            TEXT("Append sub node to root node failed"));

        funcErr = CA_ERR_NO_MEM;
        goto EXIT;
    }

EXIT:
    if (NULL != msgElementPtr)
    {
        try
        {
            msgElementPtr.Release();
        }
        catch (...)
        {
        }
    }

    return funcErr;
}

CA_DECLARE(const TCHAR*) CA_XFGetFName(CAXF *pXF)
{
    return pXF->szXFName;
}
