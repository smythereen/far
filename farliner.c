/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into buffer
 ******************************************************************************/
#include	"farlocal.h"

/* Local Variables */

static	int	FD = -1;

void	Close_Line( char * filename )
{
	FILE *	fd;
	if( ( FD == -1 || strcmp( filename, File_Table[ FD ].name ) != 0 ) &&
	    ( FD = Is_File_Open( filename ) ) == -1 ) return;
	Trace(	FAR_TRC_LVL, "Close_Line( %s:%d )", filename, FD );
	fd = File_To_FILE( FD );
	if( fd != (FILE *)0 )	FILE_Close( fd );
	else if(  FD != -1  )	File_Close( FD );
	FD = -1;
}

char *	Read_Line( char * filename, char * buffer, u32 length )
{
	FILE *	fd;
	if( filename == 0 || *filename == 0 ) {
		Trace(	0,
			"Error Read_Line( \"%s\", buf, %u ): "
			"No File Name!",
			filename,
			length
		);
		errno = ENOENT;
		return( (char *)0 );
	}
	if( buffer == 0 || length == 0 ) {
		Trace(	0,
			"Error Read_Line( \"%s\", \"\", %u ): "
			"No Buffer/Length!",
			filename,
			length
		);
		errno = EFAULT;
		return( (char *)0 );
	}
	Trace(	FAR_TRC_LVL,
		"Enter Read_Line( \"%s\", 0x%X, %d )",
		filename,
		buffer,
		length
	);
	if( filename[ 0 ] == '-' && filename[ 1 ] == 0 ) {
		if( ( fd = File_To_FILE( 0 ) ) == (FILE *)0 ) return( (char *)0 );
		if( FILE_Gets( fd, buffer, length ) != (char *)0 ) return( buffer );
		return( (char *)0 );
	}
	if( ( FD == -1 || strcmp( filename, File_Table[ FD ].name ) != 0 ) &&
	    ( FD = Is_File_Open( filename ) ) == -1 ) {
		FD = File_Open( filename, O_RDONLY, 0666 );
	}

	if( ( fd = File_To_FILE( FD ) ) == (FILE *)0 ) return( (char *)0 );

	if( FILE_Gets( fd, buffer, length ) != (char *)0 ) return( buffer );

	Close_Line( filename );

	return( (char *)0 );
}

