/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2008-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Converts from	(	FILE *	file_descriptor )
 *		 to	(	int	file_descriptor )
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<fcntl.h>

int	FILE_To_File( FILE * fd )
{
	int		FD;
	fard_t	*	f;
	char		buf[ MAX_PATH ];

	if( fd == (FILE *)0 ) return( -1 );

        for( FD = 0; FD < MAX_FD; FD++ ) {
                if( fd == File_Table[ FD ].fptr ) return( FD );
        }

	FD = fileno( fd );
	if( FD >= 0 && FD < MAX_FD && File_Table[ FD ].fptr != fd ) {
		f = &File_Table[ FD ];
		f->fd = FD;
		f->fptr = fd;
		f->flags = fcntl( FD, F_GETFL );
		fstat( FD, &f->s );
		sprintf( buf, "/proc/self/fd/%u", FD );
		readlink( buf, f->name, sizeof( f->name ) );
	}
	return( FD );
}
