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

#ifndef _CA_XF_H_
#define _CA_XF_H_ 1

#include "ca_types.h"

#define CA_XF_XSL_FNAME   TEXT("msg_style.xsl")
#define CA_XF_XSL_HDR     TEXT("type='text/xsl' href='msg_style.xsl'")

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

CA_DECLARE(const TCHAR*) CA_XFGetFName(CAXF *pXF);

CA_DECLARE(CAErrno) CA_XFSave(CAXF *pXF);

CA_DECLARE(CAErrno) CA_XFAddNode(CAXF *pXF, CAXFNode *pNode);

#endif /* !defined(_CA_XF_H_) */
