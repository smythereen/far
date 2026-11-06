/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	File Access Routines
 *
 *	This file defines common structures, variables, and routines
 *	need for FAR functions.
 ******************************************************************************/
#ifndef	FARLOCAL_H
#define	FARLOCAL_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<errno.h>
#include	<fcntl.h>
#include	<stdio.h>
#include	<sys/errno.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<sys/time.h>
#include	<unistd.h>
#include	<stdarg.h>

#include	<far.h>

#define		MAX_WORDS	50
#define		MAX_BUFFER_SIZE	4096

extern	fard_t	File_Table[ ];

#if	defined( __cplusplus )
}
#endif

#endif
