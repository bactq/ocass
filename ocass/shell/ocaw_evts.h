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

#ifndef _OCAW_EVTS_H_
#define _OCAW_EVTS_H_ 1

BOOL OCAS_OnMainDlgCmdEvt(HWND hWnd, UINT wParam, LPARAM lParam, 
                          DWORD dwCtlId);

BOOL OCAS_OnMainDlgClose(HWND hWnd);
BOOL OCAS_OnMainDlgInit(HWND hWnd);

BOOL OCAS_ShowTrayPopMenu(HWND hWnd);

BOOL OCAS_MainDlgShow(HWND hWnd);
BOOL OCAS_MainDlgHide(HWND hWnd);

BOOL OCAS_OnWrkDescChange(HWND hWnd);

#endif /* !defined(_OCAW_EVTS_H_) */

