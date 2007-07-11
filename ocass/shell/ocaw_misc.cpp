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

#include <shlobj.h>
#include "ca_str.h"
#include "ocaw_misc.h"

BOOL OCAS_AppendMenuItem(HMENU hMenu, UINT nMenuId, TCHAR *pszName, 
                         BOOL bDefault, BOOL bDisabled)
{
    MENUITEMINFO menuItemInfo = {0};

    menuItemInfo.wID = nMenuId;
    menuItemInfo.cbSize = sizeof(menuItemInfo);
    menuItemInfo.fMask = MIIM_ID|MIIM_TYPE|MIIM_STATE;
    if (NULL != pszName)
    {
        if (bDefault) 
        {
            menuItemInfo.fState = MFS_DEFAULT;
        }
        if (bDisabled)
        {
            menuItemInfo.fState |= MFS_DISABLED;
        }

        menuItemInfo.fType = MFT_STRING;
        menuItemInfo.dwTypeData = pszName;
    }
    else
    {
        menuItemInfo.fType = MFT_SEPARATOR;
    }

    return InsertMenuItem(hMenu, nMenuId, FALSE, &menuItemInfo);
}

BOOL OCAS_AppendMenuItems(HMENU hMenu, const OCASMenuItem *pItems, 
                          DWORD dwItemsCnt)
{
    const OCASMenuItem *pItem;
    DWORD dwItemId;
    BOOL bResult;
    BOOL bRetVal = TRUE;

    for (dwItemId = 0; dwItemId < dwItemsCnt; dwItemId++)
    {
        pItem = &pItems[dwItemId];
        if (NULL == pItem)
        {
            continue;
        }

        bResult = OCAS_AppendMenuItem(hMenu, pItem->nMenuId, 
            pItem->pszName, pItem->bDefault, pItem->bDisabled);
        if (!bResult)
        {
            bRetVal = FALSE;
        }
    }

    return bRetVal;
}

BOOL OCAS_SetDlgItemTxt(HWND hDlg, UINT nItemId, const TCHAR *pszTxt)
{
    HWND hDlgItem;

    hDlgItem = GetDlgItem(hDlg, nItemId);
    if (NULL == hDlgItem)
    {
        return FALSE;
    }

    SendMessage(hDlgItem, WM_SETTEXT, NULL, 
        (LPARAM)(NULL == pszTxt ? TEXT("") : pszTxt));
    return TRUE;
}

BOOL OCAS_SelPath(HWND hParentWnd, TCHAR *pszBuf, DWORD dwBufCnt)
{
    ITEMIDLIST *pBrowsed;
    BROWSEINFO browseInfo = {0};
    TCHAR szPath[MAX_PATH];
    BOOL bResult;
    int nResult;

    browseInfo.hwndOwner = hParentWnd;
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS;
    browseInfo.lpszTitle = TEXT("Open folder");
    browseInfo.lpfn = NULL;
    browseInfo.lParam = (LPARAM)hParentWnd;
    pBrowsed = SHBrowseForFolder(&browseInfo);
    if (NULL == pBrowsed)
    {
        return FALSE;
    }

    bResult = SHGetPathFromIDList(pBrowsed, szPath);
    if (!bResult)
    {
        return FALSE;
    }

    nResult = CA_SNPrintf(pszBuf, dwBufCnt, TEXT("%s"), szPath);
    return (0 >= nResult ? FALSE : TRUE);
}

BOOL OCAS_SelFile(HWND hParentWnd, const TCHAR *pszFilter, 
                  TCHAR *pszBuf, DWORD dwBufCnt)
{
    OPENFILENAME openFName = {0};
    TCHAR szFileBuf[MAX_PATH];
    BOOL bResult;
    int nResult;

    szFileBuf[0] = '\0';
    openFName.lStructSize = sizeof(openFName);
    openFName.hwndOwner = hParentWnd;
    openFName.lpstrFilter = pszFilter;
    openFName.lpstrInitialDir = NULL;
    openFName.lpstrTitle = TEXT("Open");
    openFName.nMaxFile = sizeof(szFileBuf) / sizeof(szFileBuf[0]);
    openFName.lpstrFile = szFileBuf;
    openFName.lpfnHook = NULL;
    openFName.Flags |= OFN_FILEMUSTEXIST;
    openFName.Flags |= OFN_PATHMUSTEXIST;
    openFName.Flags |= OFN_ENABLESIZING;
    openFName.Flags |= OFN_HIDEREADONLY;
    bResult = GetOpenFileName(&openFName);
    if (!bResult)
    {
        return FALSE;
    }

    nResult = CA_SNPrintf(pszBuf, dwBufCnt, TEXT("%s"), szFileBuf);
    return (0 >= nResult ? FALSE : TRUE);
}
