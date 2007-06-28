/**
 *
 */

#include "cs_ntr.h"
#include "ca_inject.h"
#include "cs_nti.h"

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
}
