/*
 * OCASS - Microsoft Office Communicator Assistant 
 *         (http://code.google.com/p/ocass/)
 *
 * Copyright (C) 2007 Le Xiongjia 
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Le Xiongjia (webmaster@lexx.51.net xiongjia_le@sonic.com)
 *
 */

#ifndef _CS_WRK_H_
#define _CS_WRK_H_ 1

#include "ca_types.h"
#include "ca_cfg.h"
#include "cs_proto.h"

typedef struct _CSWrk CSWrk;

CAErrno CS_WrkStart(HMODULE hLib, CACfgDatum *pCfgDatum, 
                    CSWrk **pCSWrk);

CAErrno CS_WrkStop(CSWrk *pCSWrk);

BOOL CS_WrkIsPause(CSWrk *pCSWrk);

CAErrno CS_WrkTouchSem(CSWrk *pCSWrk, DWORD dwTouchCnt);

const CASpyDatum* CS_WrkGetSD(const CSWrk *pCSWrk);

void CS_WrkAddRawBuf(CSWrk *pCSWrk, CSProtoBuf *pProtoBuf, 
                     CSProtoType protoType);

#endif /* !defined(_CS_WRK_H_) */
