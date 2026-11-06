/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Closes An Open File & Descriptor
 ******************************************************************************/
#include	"farlocal.h"

int	FILE_Close( FILE * fd )
{
	return( File_Close( FILE_To_File( fd ) ) );
}
