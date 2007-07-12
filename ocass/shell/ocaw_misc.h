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

#ifndef _OCAW_MISC_H_
#define _OCAW_MISC_H_ 1

#include "ca_types.h"

typedef struct OCAS_MenuItem
{
    TCHAR *pszName;
    UINT nMenuId;
    BOOL bDefault;
    BOOL bDisabled;
} OCASMenuItem;

BOOL OCAS_AppendMenuItem(HMENU hMenu, UINT nMenuId, TCHAR *pszName, 
                         BOOL bDefault, BOOL bDisabled);

BOOL OCAS_AppendMenuItems(HMENU hMenu, const OCASMenuItem *pItems, 
                          DWORD dwItemsCnt);

typedef struct _OCAS_ComboBoxItem
{
    const TCHAR *pszTxt;
    void *pData;
} OCASComboBoxItem;

BOOL OCAS_ComboBoxSetSelWithId(HWND hComboBox, int nSelId);
BOOL OCAS_ComboBoxSetSelWithData(HWND hComboBox, void *pData, 
                                 int nDefault);

BOOL OCAS_ComboBoxGetCurSelItemData(HWND hComboBox, void **pData);


BOOL OCAS_ComboBoxDelAllItems(HWND hComboBox);
BOOL OCAS_ComboBoxAddItem(HWND hComboBox, const OCASComboBoxItem *pItem);
BOOL OCAS_ComboBoxAddItems(HWND hComboBox, const OCASComboBoxItem *pItems, 
                           DWORD dwItemsCnt);

BOOL OCAS_SetDlgItemTxt(HWND hDlg, UINT nItemId, const TCHAR *pszTxt);
BOOL OCAS_GetDlgItemTxt(HWND hDlg, UINT nItemId, 
                        TCHAR *pszTxtBuf, DWORD dwBufCnt);


BOOL OCAS_SelPath(HWND hParentWnd, TCHAR *pszBuf, DWORD dwBufCnt);

BOOL OCAS_SelFile(HWND hParentWnd, const TCHAR *pszFilter, 
                  TCHAR *pszBuf, DWORD dwBufCnt);

BOOL OCAS_GetDlgCheckBoxState(HWND hDlg, UINT nCtrlItem, 
                              BOOL *pbState);

BOOL OCAS_SetDlgCheckBoxState(HWND hDlg, UINT nCtrlItem, 
                              BOOL bState);

#endif /* !defined(_OCAW_MISC_H_) */

