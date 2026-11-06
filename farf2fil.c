/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2008-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Converts from	(	int	file_descriptor )
 *		 to	(	FILE *	file_descriptor )
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<fcntl.h>

FILE *	File_To_FILE( int FD )
{
	fard_t	*	p;
	char	*	m;
	char		buf[ MAX_PATH ];

	if( FD < 0 ) return( (FILE *)0 );

	if( FD >= MAX_FD ) return( fdopen( FD, "r+" ) );

	p = &File_Table[ FD ];

	if( p->fd == FD && p->fptr != (FILE *)0 ) return( p->fptr );

	p->fd = FD;
	if( fcntl( FD, F_GETFL ) == -1 ) return( (FILE *)0 );

	p->flags = fcntl( FD, F_GETFL );

	switch( p->flags & O_ACCMODE ) {
	default:
	case	O_RDONLY:				m = "r";
							break;
	case	O_WRONLY:
		switch( p->flags & O_APPEND ) {
		default:				m = "w";
							break;
		case	O_APPEND:			m = "a";
							break;
		}
		break;

	case	O_RDWR:
		switch( p->flags & (O_APPEND|O_TRUNC|O_CREAT) ) {
		case	0:				m = "r+";
							break;
		case	O_TRUNC|O_CREAT:
		case	O_CREAT:
		case	O_TRUNC:			m = "w+";
							break;
		case	O_APPEND:
		default:				m = "a+";
							break;
		}
		break;
	}
	p->fptr = fdopen( FD, m );
	fstat( FD, &p->s );
	sprintf( buf, "/proc/self/fd/%u", FD );
	readlink( buf, p->name, sizeof( p->name ) );
	return( p->fptr );
}
