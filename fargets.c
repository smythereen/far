/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into buffer
 *		Strips/Ignores Blank & Comment Lines Also !!!!
 ******************************************************************************/
#include	"farlocal.h"

char *	File_Gets( int FD, char * buf, u32 len )
{
	return( FILE_Gets( File_To_FILE( FD ), buf, len ) );
}
