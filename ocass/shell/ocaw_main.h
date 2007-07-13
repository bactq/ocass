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

#ifndef _OCAW_MAIN_H_
#define _OCAW_MAIN_H_ 1

#include "ca_types.h"
#include "ca_cfg.h"
#include "ocaw_proc.h"
#include "ocaw_misc.h"
#include "resource.h"

#define OCAW_MSG_NOTIFY_ICON    (WM_USER + 101)
#define OCAW_MSG_WAKEUP         (WM_USER + 102)
#define OCAW_MSG_STATE_CHANGE   (WM_USER + 103)

#define OCAW_MAIN_NOTIFY_ICON_ID        (101)

const OCASMenuItem g_popMenuItems[] = 
{    
    { TEXT("&Browse history folder ..."), 
      ID_POPMENU_BROWSE_HISTORY, FALSE , FALSE }, 
    { NULL, -1, FALSE , FALSE }, 
    { TEXT("&Open"), ID_POPMENU_OPEN, FALSE , FALSE }, 
    { TEXT("E&xit"), ID_POPMENU_CLOSE, TRUE, FALSE }
};

const OCASComboBoxItem g_logModComboItems[] =
{
    { TEXT("Error"),              (void*)CA_LOGMOD_ERR },
    { TEXT("Warning"),            (void*)CA_LOGMOD_WARN },
    { TEXT("None"),               (void*)CA_LOGMOD_NONE },
    { TEXT("Debug"),              (void*)CA_LOGMOD_DBG },
    { TEXT("Debug (archive)"),    (void*)CA_LOGMOD_DBG_ARC }
};

const HINSTANCE CAS_MGetAppInst(void);
OCAWProc* CAS_MGetProcPtr(void);

#endif /* !defined(_OCAW_MAIN_H_) */
