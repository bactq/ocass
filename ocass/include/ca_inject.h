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

#ifndef _CA_INJECT_H_
#define _CA_INJECT_H_ 1

#include "ca_types.h"


CA_DECLARE(CAErrno) CA_ReplaceAPI(const TCHAR *pszModName, 
                const TCHAR *pszAPIName, PROC pNewFunc, PROC *pOldFunc);

CA_DECLARE(void) CA_ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
        PROC pfnCurrent, PROC pfnNew, BOOL fExcludeAPIHookMod);

#endif /* !defined(_CA_INJECT_H_) */
