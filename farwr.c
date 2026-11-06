/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Writes To A File
 ******************************************************************************/
#include	"farlocal.h"

int	File_Write(	int	FD, 
			void *	buf, 
			u32	size
	)
{
	int	r;
	int	t;

	if( FD == -1 ) {
		errno = EBADF;
		return( -1 );
	}
	if( size == 0 ) return( 0 );
	if( buf == 0 ) {
		errno = EFAULT;
		if( FD == 2 ) return( -1 );
		Trace(	0,
			"Error File_Write( %u, buf, %u ): No Buffer !",
			FD,
			size
		);
		return( -1 );
	}
	t =  0;
	while( ( r = write( FD, buf, size ) ) != size ) {
		if( r > 0 ) {
			if( FD != 2 ) {
				Trace(	0,
					"Error File_Write( %u, buf, %u ): "
					"Incomplete Write %u < %u !",
					FD,
					r,
					size
				);
			}
			t += r;
			size -= r;
			buf = &((char *)buf)[ r ];
			continue;
		} else if ( r == 0 ) {
			continue;
		}

		switch( errno ) {

		case EAGAIN:	return( 0 );

		case EINTR:	continue;

		default:
				if( FD == 2 ) return( -1 );
				Trace(	0,
					"Error File_Write( %u, buf, %u ): %s",
					FD,
					size,
					strerror( errno )
				);
				return( -1 );
		}
	}
	return( ( r + t ) );
}
