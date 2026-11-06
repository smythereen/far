/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Closes An Open File & Descriptor
 ******************************************************************************/
#include	<dirent.h>
#include	"farlocal.h"

static	int	close_error( int FD, char * err )
{
	Trace(	0,
		"ERROR(%u): File_Close( %u ) - %s",
		errno,
		FD,
		( ( err != 0 && *err != 0 ) ? err : strerror( errno ) )
	);
	return( -1 );
}

int	File_Close( int	FD )
{
	int		r;
	fard_t	*	p;
	DIR	*	d;

	Trace(	FAR_TRC_LVL,
		"File_Close(%u) = %s",
		FD,
		( ( FD < 0 || FD > MAX_FD ) ? "N/A" : File_Table[ FD ].name )
	);

	if( FD < 0 || FD > MAX_FD ) return( -1 );

	p = &File_Table[ FD ];
	r = -1;
	if( File_Is_Directory( FD ) ) {
		d = (DIR *)p->fptr;
		while( ( r = closedir( d ) ) == -1 ) {
			switch( errno ) {
			case EINTR:	continue;
			default:	close_error( FD, 0 );
			case EBADF:	break;
			}
			break;
		} 
	} else if( p->fptr != 0 ) {
		while( ( r = fclose( p->fptr ) ) != 0 ) {
			switch( errno ) {
			case EINTR:	continue;
			default:	close_error( FD, 0 );
					while( ( r = close( FD ) ) == -1 ) {
						switch( errno ) {
						case EINTR:	continue;
						default:	close_error( FD, 0 );
						case EBADF:	break;
						}
					}
					break;
			}
			break;
		} 
	} else {
		while( ( r = close( FD ) ) == -1 ) {
			switch( errno ) {
			case EINTR:	continue;
			default:	close_error( FD, 0 );
			case EBADF:	break;
			}
			break;
		} 
	}
	memset( p, 0, sizeof( fard_t ) );
	p->fd = -1;
	return( r );
}
