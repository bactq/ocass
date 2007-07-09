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
