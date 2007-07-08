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

#ifndef _CA_TYPES_H_
#define _CA_TYPES_H_ 1

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "ca_errno.h"

#if defined(__cplusplus)
#define CA_DECLARE(_type)  __declspec(dllexport) _type __stdcall
#define CA_DECLARE_NOSTD(_type)  __declspec(dllexport) _type
#else
#define CA_DECLARE(_type)  extern "C" __declspec(dllexport) _type __stdcall
#define CA_DECLARE_NOSTD(_type)  extern "C" __declspec(dllexport) _type 
#endif /* defined(__cplusplus) */

#define CA_DECLARE_DYL(_type)  extern "C" __declspec(dllexport) _type 

#define CA_SRC_MARK     __FILE__, __LINE__

#endif /* !defined(_CA_TYPES_H_) */
