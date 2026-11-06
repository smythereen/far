/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Detects If File Is Already Open And Returns Its File Descriptor
 ******************************************************************************/
#include	"farlocal.h"

int	Is_File_Open( char * file )
{
	int		FD;
	struct	stat	x;

	if( stat( file, &x ) != 0 ) return( -1 );
	for( FD=0 ; FD < MAX_FD; FD++ ) {
		if( x.st_dev == File_Table[ FD ].s.st_dev &&
		    x.st_ino == File_Table[ FD ].s.st_ino ) return( FD );
	}
	return( -1 );
}
