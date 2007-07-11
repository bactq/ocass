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

#include "libocc.h"
#include "liboca.h"

#include "ocac_proc.h"
#include "ocac_wrk.h"
#include "ca_ofc.h"

int main(int argc, char **argv)
{
    OCACProc ocacProc;
    CAErrno caErr;
    int nProcExit = CA_PROC_EXIT_OK;

    caErr = CAC_PStartup(argc, argv, &ocacProc);
    if (CA_ERR_SUCCESS != caErr)
    {
        return CA_PROC_EXIT_INIT_FAILED;
    }

    nProcExit = OCAC_PWrk(&ocacProc);
    CAC_PCleanup(&ocacProc);
    return nProcExit;
}
