/**
 *
 */

#ifndef _CH_INNER_H_
#define _CH_INNER_H_ 1

typedef struct _CH_HOOK_DATUM
{
    HHOOK hInjectWndHook;
} CHHDatum;


void        CH_HDatumInit(void);
CHHDatum*   CH_HDatumPtrGet(void);

LRESULT WINAPI CH_HookCBTProc(int nCode, WPARAM wParam, LPARAM lParam);

void CH_OnInject(void);
void CH_OnInjectComplete(CHHDatum *pHDatum);

#endif /* !defined(_CH_INNER_H_) */

