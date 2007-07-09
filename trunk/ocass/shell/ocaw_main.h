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

#ifndef _OCAW_MAIN_H_
#define _OCAW_MAIN_H_ 1

#include "ca_types.h"
#include "ocaw_proc.h"
#include "ocaw_misc.h"
#include "resource.h"

#define OCAW_MSG_NOTIFY_ICON    (WM_USER + 101)

#define OCAW_MAIN_NOTIFY_ICON_ID        (101)

const OCASMenuItem g_popMenuItems[] = 
{
    { TEXT("&Open") , ID_POPMENU_OPEN, TRUE, FALSE }, 
    { TEXT("E&xit") , ID_POPMENU_CLOSE, TRUE, FALSE }, 
};

const HINSTANCE CAS_MGetAppInst(void);
OCAWProc* CAS_MGetProcPtr(void);

#endif /* !defined(_OCAW_MAIN_H_) */
