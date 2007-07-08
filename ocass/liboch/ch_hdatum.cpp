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
