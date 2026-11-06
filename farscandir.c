/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Recursively Scans Directories
 ******************************************************************************/
#include	"farlocal.h"
#include	<dirent.h>

typedef struct  dev_ino_s{
        dev_t   d;
        ino_t   i;
}       dev_ino_t;

static	int	counter=0;
static	dev_ino_t	di[ 4000 ];
extern	int	errno;

int	ScanDir( char * dir, int l_flag, void 	(*func)( char * dir, char * link )  )
{
struct	dirent	**	namelist;
struct	stat		s;
	char	*	d;
	char		path[ MAX_PATH ];
	char		link[ MAX_PATH ];
	int		i, n;

	i = scandir( dir, &namelist, 0, alphasort );
	if( i < 0 ) {
		Trace( FAR_TRC_LVL, "ScanDir Error: %s", strerror( errno ) );
		return( -1 );
	} 
	for( n = 0; n < i && counter < 4000; n++ ){
		d = namelist[ n ]->d_name;
		memset( link, 0, MAX_PATH );
		if( d[ 0 ] == '.' && ( d[ 1 ] == '.' || d[ 1 ] == 0 ) ) continue;
		sprintf( path, "%s/%s", dir, d );
		if( l_flag ){
			if( lstat( path, &s ) == -1 ) continue;
		} else {
			if( stat( path, &s ) == -1 ) continue;
		}
		counter++;
		di[ counter ].d = s.st_dev;
		di[ counter ].i = s.st_ino;
		if( S_ISDIR( s.st_mode ) ){
			ScanDir( path, l_flag, func );
			continue;
		}
		if( S_ISLNK( s.st_mode ) ){
			if( readlink( path, link, sizeof( link ) ) == -1 ) {
				d = NULL;
			} else {
				d = link;
			}
		} else {
			d = NULL;
		} 
		if( func == NULL ){
			if( d == NULL ){
				printf( "%s\n", path);
			} else {
				printf( "%s -> %s\n", path, d);
			}
		} else {
			func( path, d );
		}
		free( namelist[ n ] );
	}
	free( namelist );
	return 0;
}
