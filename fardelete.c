/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Recursive Delete
 ******************************************************************************/
#include	"farlocal.h"
#include	<dirent.h>

static	int	counter=0;
extern	int	errno;

int	Delete( char * dir )
{
struct	dirent	*	entry;
	DIR	*	d_stream;
	char		path[ MAX_PATH ];

	if( dir[ 0 ] == '/' && dir[ 1 ] == '0' ){
		FatalError( "FAR Delete ERROR: THIS WILL REMOVE ENTIRE FILE SYSTEM!!!\n" );
	}
	if( remove( dir ) == 0 ) return( 0 );
	switch( errno ){
	case ENOENT:
		return( 0 );
	case ENOTEMPTY:
		break;
	default:
		EventLog( "Error: FAR Delete Couldn't Remove %s - %s\n", dir, strerror( errno ) );
		Trace( 0, "Error: FAR Delete Couldn't Remove %s - %s\n", dir, strerror( errno ) );
		return( -1 );
	}
	if( counter > 7 ){
		EventLog( "Error: FAR Delete Exceeding Maximum Level of Recursion!\n" );
		Trace( 0, "Error: FAR Delete Exceeding Maximum Level of Recursion!\n" );
		return( -1 );
	}
	if( ( d_stream = opendir( dir ) ) == NULL ){
		EventLog( "Error: FAR Delete Couldn't Open Directory %s - %s!\n", dir, strerror( errno ) );
		Trace( 0, "Error: FAR Delete Couldn't Open Directory %s - %s!\n", dir, strerror( errno ) );
		return( -1 );
	}
	counter++;
	while( ( entry = readdir( d_stream ) ) != NULL ) {
		if( ( strcmp( entry->d_name, ".")  == 0 ) ||  ( strcmp( entry->d_name, "..")  == 0 ) ) continue;
		sprintf( path, "%s/%s", dir, entry->d_name );
		Delete( path );
	}
	closedir( d_stream );
	--counter;
	if( remove( dir ) != 0 ){
		EventLog( "Error: FAR Delete Remove %s Failed! %s", dir, strerror( errno ) ); 
		Trace( 0, "Error: FAR Delete Remove %s Failed! %s", dir, strerror( errno ) ); 
		return( -1 );
	}
	return 0;
}
