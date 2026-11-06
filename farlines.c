/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into buffer
 ******************************************************************************/
#include	"farlocal.h"

char **	Read_All_Lines( char * filename, char * buffer, s32 length )
{
	char **	av;
	char *	b;
	s32	l;
	s32	s;

	if( filename == 0 || *filename == 0 ) {
		Trace(	0,
			"Error Read_All_Lines( \"%s\", buf, %u ): "
			"No File Name!",
			filename,
			length
		);
		errno = ENOENT;
		return( (char **)0 );
	}
	if( buffer == 0 || length < 8 ) {
		Trace(	0,
			"Error Read_All_Lines( \"%s\", 0x%X, %d ): "
			"No Buffer Pointer/Length!",
			filename,
			buffer,
			length
		);
		errno = EFAULT;
		return( (char **)0 );
	}

	Trace(	FAR_TRC_LVL,
		"Enter Read_All_Lines( \"%s\", 0x%X, %d )",
		filename,
		buffer,
		length
	);

	b = buffer;
	l = length;
	av = Vector_Buffer( &b, &l );

	Trace(	FAR_TRC_LVL,
		"Before Read_Line( \"%s\", 0x%X, %d )",
		filename,
		b,
		l
	);
	while( Read_Line( filename, b, l ) != (char *)0 ) {
		/*
		 *	Remove Trailing New Line Characters
		 */
		for( s = strlen( b ) - 1; b[ s ] == '\n'; s-- ) b[ s ] = 0;
		/*
		 *	Add Buffer Pointer To Argument Vector
		 */
		av = Add_To_Vector( av, &b, &l );
		if( errno == 0 ) continue;
		Close_Line( filename );
		break;
	}
	Trace(	FAR_TRC_LVL,
		"Exit  Read_All_Lines( \"%s\", 0x%X, %d ) = 0x%X",
		filename,
		buffer,
		length,
		av
	);
	return( av );
}
