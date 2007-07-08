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

#include "cs_ntr.h"
#include "cs_nti.h"

int WSAAPI CS_sendVer1(SOCKET s, const char *buf, 
                       int len, int flags)
{
    CSNetWrkApiSlot *pApiSlot = CS_NtWrkGetSlot(CS_NETWRK_API_SND_VER1);
    CS_sendFunc pOldSendV1Func = NULL;
    int nRetVal = SOCKET_ERROR;

    pApiSlot->bInUsed = TRUE;
    pOldSendV1Func = (CS_sendFunc)pApiSlot->pOldProc;
    if (NULL == pOldSendV1Func)
    {
        nRetVal = SOCKET_ERROR;
        goto EXIT;
    }

    nRetVal = pOldSendV1Func(s, buf, len, flags);
    if (SOCKET_ERROR != nRetVal)
    {
        CS_OnNtWrkIO(s, buf, nRetVal, CS_NETWRK_API_SND_VER1);
    }
EXIT:
    pApiSlot->bInUsed = FALSE;
    return nRetVal;
}


int WSAAPI CS_send(SOCKET s, const char *buf, 
                   int len, int flags)
{
    CSNetWrkApiSlot *pApiSlot = CS_NtWrkGetSlot(CS_NETWRK_API_SND);
    CS_sendFunc pOldSendFunc = NULL;
    int nRetVal = SOCKET_ERROR;

    pApiSlot->bInUsed = TRUE;
    pOldSendFunc = (CS_sendFunc)pApiSlot->pOldProc;
    if (NULL == pOldSendFunc)
    {
        nRetVal = SOCKET_ERROR;
        goto EXIT;
    }

    nRetVal = pOldSendFunc(s, buf, len, flags);
    if (SOCKET_ERROR != nRetVal)
    {
        CS_OnNtWrkIO(s, buf, nRetVal, CS_NETWRK_API_SND);
    } 
EXIT:
    pApiSlot->bInUsed = FALSE;
    return nRetVal;
}

int WSAAPI CS_WSARecv(SOCKET s, LPWSABUF lpBuffers, 
        DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, 
        LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, 
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    CSNetWrkApiSlot *pApiSlot = CS_NtWrkGetSlot(CS_NETWRK_API_WSARCV);
    CS_WSARecvFunc pOldWSARcvFunc = NULL;
    int nRetVal = SOCKET_ERROR;

    pApiSlot->bInUsed = TRUE;
    pOldWSARcvFunc = (CS_WSARecvFunc)pApiSlot->pOldProc;
    if (NULL == pOldWSARcvFunc)
    {
        nRetVal = SOCKET_ERROR;
        goto EXIT;
    }

    nRetVal = pOldWSARcvFunc(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, 
            lpFlags, lpOverlapped, lpCompletionRoutine);
    if (SOCKET_ERROR != nRetVal)
    {
        if (0 < dwBufferCount)
        {
            WSABUF *pBuf = lpBuffers;

            /* FIXME if not one buf and use rcv data len */
            CS_OnNtWrkIO(s, pBuf->buf, pBuf->len, CS_NETWRK_API_WSARCV);
        }
    }
EXIT:
    pApiSlot->bInUsed = FALSE;
    return nRetVal;
}
