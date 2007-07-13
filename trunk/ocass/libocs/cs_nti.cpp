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

#include "cs_ntr.h"
#include "ca_inject.h"
#include "cs_nti.h"
#include "cs_proto.h"

static CSNetWrkApiList CS_NETWRK_APIS[] = 
{
    { 
        CS_NETWRK_API_SND_VER1, 
        TEXT("wsock32.dll"),  
        TEXT("send"),  
        (PROC)CS_sendVer1 
    },
    { 
        CS_NETWRK_API_SND, 
        TEXT("ws2_32.dll"),  
        TEXT("send"), 
        (PROC)CS_send
    },
    { 
        CS_NETWRK_API_WSARCV, 
        TEXT("ws2_32.dll"),  
        TEXT("WSARecv"), 
        (PROC)CS_WSARecv
    }
};

static CSNetWrkApiSlot g_netwrkApiSlot[CS_NETWRK_API_COUNT] = {0};

CSNetWrkApiSlot* CS_NtWrkGetSlot(int nIndex)
{
    return &g_netwrkApiSlot[nIndex];
}

CAErrno CS_NtWrkApiAttach(void)
{
    CSNetWrkApiSlot *pApiSlot;
    CSNetWrkApiList *pNetWrkApi;
    const int nAPIsCount = sizeof(CS_NETWRK_APIS) / sizeof(CS_NETWRK_APIS[0]);
    CAErrno caErr;
    PROC pOldProc;
    int i;

    /* XXX FIXME this code is not thread safe */
    for (i = 0; i < nAPIsCount; i++)
    {
        pNetWrkApi = &CS_NETWRK_APIS[i];
        pApiSlot = &g_netwrkApiSlot[pNetWrkApi->nSlotId];
        pApiSlot->bInUsed = FALSE;
        pApiSlot->bHooked = FALSE;

        pApiSlot->pNewProc = pNetWrkApi->pNewProc;
        pApiSlot->pszAPIName = pNetWrkApi->pszAPIName;
        pApiSlot->pszModName = pNetWrkApi->pszModName;
        pApiSlot->pOldProc = NULL;

        caErr = CA_ReplaceAPI(pNetWrkApi->pszModName, 
            pNetWrkApi->pszAPIName, pNetWrkApi->pNewProc, &pOldProc);
        if (CA_ERR_SUCCESS != caErr)
        {
            continue;
        }

        pApiSlot->bHooked = TRUE;
        pApiSlot->pOldProc = pOldProc;
    }

    return CA_ERR_SUCCESS;
}

void CS_OnNtWrkIO(SOCKET netFd, const char *pBuf, 
                  int nBufLen, int nApiSlotId)
{
    CSProtoBuf protoBuf;
    BOOL bIsRcv;

    if (CS_NETWRK_API_SND == nApiSlotId || 
        CS_NETWRK_API_SND_VER1 == nApiSlotId)
    {
        bIsRcv = FALSE;
    }
    else if (CS_NETWRK_API_WSARCV == nApiSlotId)
    {
        bIsRcv = TRUE;
    }
    else
    {
        return;
    }

    protoBuf.netFd = netFd;
    protoBuf.pBuf = pBuf;
    protoBuf.nBufLen = nBufLen;
    protoBuf.bIsRcv = bIsRcv;
    protoBuf.nApiSlot = nApiSlotId;
    CS_ProtoAddBufItem(&protoBuf);
}
