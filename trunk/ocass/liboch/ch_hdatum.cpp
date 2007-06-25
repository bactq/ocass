/**
 *
 */

#include "ca_types.h"
#include "ch_inner.h"

#pragma data_seg(".och_Datum")
    CHHDatum g_chhDatum = {0};
#pragma data_seg()
#pragma comment(linker, "/section:.och_Datum,rws")

void CH_HDatumInit(void)
{
    memset(&g_chhDatum, 0, sizeof(g_chhDatum));
}

CHHDatum* CH_HDatumPtrGet(void)
{
    return &g_chhDatum;
}
