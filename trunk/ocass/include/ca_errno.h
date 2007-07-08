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

#ifndef _CA_ERRNO_H_
#define _CA_ERRNO_H_ 1

/**
 * @{
 */

typedef int CAErrno;
#define CA_ERR_SUCCESS              (0)
#define CA_ERR_EOF                  (200)

#define CA_ERR_FATAL                (-900)
#define CA_ERR_NOT_IMPL             (-901)
#define CA_ERR_BAD_ARG              (-902)
#define CA_ERR_BAD_SEQ              (-910)
#define CA_ERR_NO_MEM               (-903)
#define CA_ERR_SYS_CALL             (-905)
#define CA_ERR_TIMEOUT              (-907)
#define CA_ERR_OPT_CANCEL           (-908)
#define CA_ERR_OBJ_CANNOT_FIND      (-909)
#define CA_ERR_OBJ_NAME_TOO_LONG    (-920)

#define CA_ERR_FNAME_TOO_LONG       (-1000)
#define CA_ERR_FNAME_CANNOT_FIND    (-1001)
#define CA_ERR_FNAME_BAD            (-1002)
#define CA_ERR_FOPEN                (-1052)
#define CA_ERR_FCREATE              (-1053)
#define CA_ERR_FWRITE               (-1055)

/** @} */

/**
 * @{
 */

#define CA_PROC_EXIT_OK             (  0)
#define CA_PROC_EXIT_INIT_FAILED    ( 25)
#define CA_PROC_EXIT_ALREADY_RUN    ( 30)
#define CA_PROC_EXIT_NO_RUNNING     ( 31)
#define CA_PROC_EXIT_USEAGE         ( 50)
#define CA_PROC_EXIT_FATAL          (101)

/** @} */

/**
 * @{
 */

#define CA_THREAD_EXIT_OK             (  0)

/** @} */

#endif /* !defined(_CA_ERRNO_H_) */
