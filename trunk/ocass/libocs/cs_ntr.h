/**
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
