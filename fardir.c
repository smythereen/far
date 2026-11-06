/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Builds Directories In A File Path
 ******************************************************************************/
#include	"farlocal.h"

int	Access_Dir( char * path, int mode )
{
	char	buf[ MAX_PATH ];

	strcpy( buf, path );
	if( ( path = strrchr( buf, '/' ) ) == 0 ) return( 0 );
	*path = 0;
	if( access( buf, R_OK | W_OK | X_OK ) == 0 ) return( 1 );
	switch( errno ) {
	case EACCES:
		if( chmod( buf, ( mode | 0755 ) ) != -1 ) return( 1 );
	}
	return( 0 );
}

static	int	Create_Dir( char * path, int mode )
{
	if( mkdir( path, ( mode | 0755 ) ) != -1 ) return( 1 );
	switch( errno ) {
	case EACCES:	if( !Access_Dir( path, ( mode | 0775 ) ) ) break;
			if( mkdir( path, ( mode | 0755 ) ) != -1 ) return( 1 );
			break;
	case EEXIST:	if( access( path, R_OK | W_OK | X_OK ) != -1 ) return( 1 );
			switch( errno ) {
			case EACCES:
				if( chmod( path, ( mode | 0775 ) ) != -1 ) return( 1 );
			}
			if( chmod( path, ( mode | 0755 ) ) != -1 ) return( 1 );
	default:	break;
	}
	Trace(	0,
		"Error Create_Dir( \"%s\" ): %s",
		path,
		strerror( errno )
	);
	return( 0 );
}

static	int	Check_Dir( char * path, int mode )
{
	if( access( path, X_OK ) != -1 ) return( 1 );

	switch( errno ) {
	case ENOENT:	return( Create_Dir( path, mode ) );
	default:	Trace(	0,
				"Check_Dir( \"%s\" ): %s",
				path,
				strerror( errno )
			);
	}
	return( 0 );
}

int	Build_Dir( char * path, int mode )
{
	char *	y;
	char	buff[ MAX_PATH ];

	if( path == (char *)0 || path[0] == 0 || path[1] == 0 ) {
		Trace( 0, "Error Build_Dir( \"\" ): No Path!" );
		return( -1 );
	}

	if( Access_Dir( path, mode ) ) return( 1 ); 

	memset( buff, 0, sizeof( buff ) );
	y = buff;
	if( *path == '/' ) *y++ = *path++;
	while( *path != 0 ) {
		if( *path == '/' && !Check_Dir( buff, mode ) ) return( 0 );
		*y++ = *path++;
	}
	return( 1 );
}
