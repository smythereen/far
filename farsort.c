/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Sorts String Pointer Array
 ******************************************************************************/
#include	"farlocal.h"

char **	Sort_List( char ** lines )
{
	char **	l;
	char *	p;
	int	again;

	if( lines == 0 || lines[ 0 ] == 0 || lines[ 1 ] == 0 ) return( lines );
	again = 1;
	while( again ) {
		again = 0;
		for( l = lines; l[ 1 ] != 0; l++ ) {
			if( strcmp( l[ 0 ], l[ 1 ] ) <= 0 ) continue;
			p = l[ 0 ];
			l[ 0 ] = l[ 1 ];
			l[ 1 ] = p;
			again = 1;
		}
	}
	return( lines );
}
