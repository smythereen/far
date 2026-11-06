/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into buffer
 ******************************************************************************/
#include	"farlocal.h"

char **	GET_All_Lines( FILE * fd, char * buffer, s32 length )
{
	char **	av;
	char *	b;
	s32	l;
	s32	s;

	if( buffer == 0 || length < 8 ) {
		Trace(	0,
			"Error GET_All_Lines( 0x%08X, \"\", %u ): "
			"No Buffer Pointer/Length!",
			fd,
			length
		);
		errno = EFAULT;
		return( (char **)0 );
	}

	b = buffer;
	l = length;
	av = Vector_Buffer( &b, &l );

	while( FILE_Gets( fd, b, l ) != (char *)0 ) {
		/*
		 *	Remove Trailing New Line Characters
		 */
		for( s = strlen( b ) - 1; b[ s ] == '\n'; s-- ) b[ s ] = 0;
		/*
		 *	Add Buffer Pointer To Argument Vector
		 */
		av = Add_To_Vector( av, &b, &l );
		if( errno != 0 ) break;
	}
	return( av );
}

char **	Get_All_Lines( int FD, char * buffer, s32 length )
{
	if( FD < 0 || FD >= MAX_FD ) {
		Trace(	0,
			"Error Get_All_Lines( %d, buf, %u ): "
			"Bad File Descriptor!",
			FD,
			length
		);
		errno = EBADF;
		return( (char **)0 );
	}
	return( GET_All_Lines( File_To_FILE( FD ), buffer, length ) );
}
