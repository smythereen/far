/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *		Verifies a path
 ******************************************************************************/
#include	"farlocal.h"

#ifdef	JUNK

static	int	Create_Dir( char * path, int mode )
{
	if( mkdir( path, (mode | 0555) ) != -1 ) return( 0 ); 
	switch( errno ) {
	case EEXIST:	return( 0 );
	default:	Trace(	0,
				"Error Create_Dir( \"%s\" ): %s",
				path,
				strerror( errno )
			);
	}
	return( -1 );
}

static	int	Check_Dir( char * path, int mode )
{
	if( access( path, R_OK | X_OK ) != -1 ) return( 0 ); 
	switch( errno ) {
	case ENOENT:	return( Create_Dir( path, mode ) );
	default:	Trace(	0,
				"Check_Dir( \"%s\" ): %s",
				path,
				strerror( errno )
			);
			return( -1 );
	}
}

static	int	AccessDir( char * path )
{
	char	buf[ MAX_PATH ];
	strcpy( buf, path );
	if( ( path = strrchr( buf, '/' ) ) == 0 ) return( -1 );
	*path = 0;
	return( access( buf, R_OK ) );
}

#endif

int	Verify_Path( char * path, int flags, int mode )
{
#ifdef	JUNK
	char *	y;
	char	buff[ MAX_PATH ];
#endif

	if( path == (char *) 0 || *path == 0 ) {
		Trace( 0, "Error Verify_Path( \"\" ): No Path!" );
		return( -1 );
	}

	if( ( flags & ( O_TRUNC | O_RDWR | O_WRONLY | O_APPEND ) ) == 0 ) {
		if( access( path, R_OK ) == 0 ) return( 1 );
		switch( errno ) {
		default:
			Trace(	0,
				"Error Verify_Path( \"%s\", 0x%X, 0%o ): %s",
				path,
				flags,
				mode,
				strerror( errno )
			);
		case ENOENT:	break;
		}
		return( -1 );
	}

	if( access( path, R_OK | W_OK ) != -1 ) return( 1 ); 
	switch( errno ) {
	default:	Trace(	0,
				"Error Verify_Path( \"%s\", 0x%X, 0%o ): %s",
				path,
				flags,
				mode,
				strerror( errno )
			);
			return( -1 );

	case ENOENT:
			if( ( flags & O_CREAT ) == 0 ) return( -1 );
			break;
	}
	return( Build_Dir( path, mode ) ? 0 : -1 );
#ifdef	JUNK
	if( AccessDir( path ) != -1 ) return( 0 ); 

	memset( buff, 0, sizeof( buff ) );
	y = buff;
	if( *path == '/' ) *y++ = *path++;
	while( *path != 0 ) {
		switch( *path ) {
		case '/':	if( Check_Dir( buff, mode ) != -1 ) break;
				return( -1 );
		}
		*y++ = *path++;
	}
	return( 0 );
#endif
}
