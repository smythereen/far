/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *		Reads From A File
 ******************************************************************************/
#include	"farlocal.h"

int	File_Read(	int	FD, 
			void *	buf, 
			u32	size
	)
{
	int	r;

	if( FD == -1 ) {
		errno = EBADF;
		return( -1 );
	}
	if( size == 0 ) return( 0 );
	if( buf == 0 ) {
		Trace(	0,
			"Error File_Read( %u, buf, %u ): No Buffer !",
			FD,
			size
		);
		errno = EFAULT;
		return( -1 );
	}
	memset( buf, 0, size );
	while( ( r = read( FD, buf, size ) ) == -1 ) {
		switch( errno ) {

		case EAGAIN:	return( 0 );

		case EINTR:	continue;

		default:	Trace(	0,
					"Error File_Read( %u, buf, %u ): %s",
					FD,
					size,
					strerror( errno )
				);
				return( -1 );
		}
	}
	return( r );
}
