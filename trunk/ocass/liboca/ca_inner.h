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

#ifndef _CA_INNER_H_
#define _CA_INNER_H_ 1

#include "liboca.h"
#include "ca_types.h"
#include "ca_mm.h"
#include "ca_flock.h"

typedef struct _CART
{
    HINSTANCE hCADll;
    TCHAR szRTWrkPath[MAX_PATH];
    CRITICAL_SECTION rtCS;
    CAFLock rtPL;

    void *pRTLogCbCtx;
    CA_RTLogFunc pRTLogFunc;
} CART;

CART* CA_GetPtrRT(void);

const HINSTANCE CA_GetDllInstance(void);

#endif /* !defined(_CA_INNER_H_) */

