/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Creates Path & Opens Stream For Read/Write/Append Access
 ******************************************************************************/
#include	"farlocal.h"

FILE *	FILE_Open( char * path, int flags, ... )
{
	int	mode;
	va_list	ap;

	va_start( ap, flags );
	mode = va_arg( ap, int );
	va_end( ap );

	return( File_To_FILE( File_Open( path, flags, mode ) ) );
}
