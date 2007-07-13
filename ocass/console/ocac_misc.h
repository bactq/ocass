/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 *
 */

#ifndef _OCAC_MISC_H_
#define _OCAC_MISC_H_ 1

#include "liboca.h"
#include "libocc.h"

void OCAC_PrintMsgLine(const TCHAR *pszMsgFmt, ...);
void OCAC_PrintMsg(const TCHAR *pszMsgFmt, ...);
void OCAC_VPrintMsgLine(const TCHAR *pszMsgFmt, va_list pArgList);
void OCAC_VPrintMsg(const TCHAR *pszMsgFmt, va_list pArgList);

void OCAC_LogShow(void *pCbCtx, CARTLog *pLog);

void OCAC_Panic(const TCHAR *pszSrc, const int nSrcLine, 
                const TCHAR *pszFmt, ...);

void OCAC_Usage(void);

void OCAC_PrintCoreState(CCWrkStateDesc *pWrkState);

#endif /* !defined(_OCAC_MISC_H_) */
