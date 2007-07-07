/**
 *
 */

#ifndef _CA_XF_H_
#define _CA_XF_H_ 1

#include "ca_types.h"

#define CA_XF_XSL_HDR     TEXT("type='text/xsl' href='MsgStyle.xsl'")

typedef struct _CAXF CAXF;

typedef struct _CAXFNode
{
    const char *pszTm;
    DWORD dwCSeq;
    const char *pszCallId;

    const char *pszFrom;
    const char *pszTo;
    const char *pszMsg;
} CAXFNode;

CA_DECLARE(CAErrno) CA_XFOpen(const TCHAR *pszFName, BOOL bCreate, 
                              CAXF **pXF);

CA_DECLARE(void)    CA_XFClose(CAXF *pXF);

CA_DECLARE(CAErrno) CA_XFSave(CAXF *pXF);

CA_DECLARE(CAErrno) CA_XFAddNode(CAXF *pXF, CAXFNode *pNode);

#endif /* !defined(_CA_XF_H_) */
