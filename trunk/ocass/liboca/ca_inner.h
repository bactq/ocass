/**
 *
 */

#ifndef _CA_INNER_H_
#define _CA_INNER_H_ 1

typedef struct _CARtl
{
    /* XXX */
    TCHAR szRtlWrkPath[MAX_PATH];
} CARtl;

CARtl* CA_GetPtrRtl(void);

#endif /* !defined(_CA_INNER_H_) */

