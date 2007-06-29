/**
 *
 */

#ifndef _CH_INNER_H_
#define _CH_INNER_H_ 1

#include "ca_types.h"
#include "ca_cfg.h"

typedef struct _CH_HOOK_DATUM
{
    HHOOK hInjectWndHook;
    CACfgDatum cfgDatum;
} CHHDatum;

const HINSTANCE CH_GetDllInstance(void);

void        CH_HDatumInit(void);
CHHDatum*   CH_HDatumPtrGet(void);

LRESULT WINAPI CH_HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam);

void CH_OnInject(CHHDatum *pHDatum);
void CH_OnInjectComplete(CHHDatum *pHDatum);

#endif /* !defined(_CH_INNER_H_) */

