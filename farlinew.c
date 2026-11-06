/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Appends A Variable Length ASCII Line To A Text File
 ******************************************************************************/
#include	"farlocal.h"

int	Write_LineAP( char * filename, char * line, va_list ap )
{
	int	FD;
	int	r;
	int	s;
	char	buf[ MAX_BUFFER_SIZE ];

	FD = File_Open( filename, ( O_WRONLY | O_APPEND | O_CREAT ), 0666 );
	if( FD == -1 ) return( FD );

	s = vsprintf( buf, line, ap );
	if( s <= 0 ) {
		s = 0;
		buf[ s++ ] = '\n';
		buf[ s   ] =   0;
	} else if( buf[ s - 1 ] != '\n' ) {
		buf[ s++ ] = '\n';
		buf[ s   ] =   0;
	}

	r = File_Write( FD, buf, s );
	File_Close( FD );
	return( r );
}

int	Write_Line( char * filename, char * line, ... )
{
	int	r;
	va_list	ap;

	va_start( ap, line );
	r = Write_LineAP( filename, line, ap );
	va_end( ap );
	return( r );
}
