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

#ifndef _CS_NTR_H_
#define _CS_NTR_H_ 1

#include <winsock2.h>
#include <windows.h>

typedef int (WSAAPI *CS_WSARecvFunc)(SOCKET s, LPWSABUF lpBuffers, 
            DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, 
            LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, 
            LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef int (WSAAPI *CS_sendFunc)(SOCKET s, const char *buf, 
             int len, int flags);


int WSAAPI CS_sendVer1(SOCKET s, const char *buf, 
                        int len, int flags);

int WSAAPI CS_send(SOCKET s, const char *buf, 
                    int len, int flags);

int WSAAPI CS_WSARecv(SOCKET s, LPWSABUF lpBuffers, 
        DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, 
        LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped, 
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#endif /* !defined(_CS_NTR_H_) */
