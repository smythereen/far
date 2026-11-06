/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Detects Comment Characters At Start Of String
 ******************************************************************************/
#include	"farlocal.h"

static	char	Comment_Characters[ 8 ] = "#~!";

int	Is_Comment( char * p )
{
	if( Comment_Characters[ 0 ] == 0 ) return( 0 );

	while( isspace( *p ) ) p++;

	if(	*p == 0				||
		*p == Comment_Characters[ 0 ]	||
		*p == Comment_Characters[ 1 ]	||
		*p == Comment_Characters[ 2 ]	||
		*p == Comment_Characters[ 3 ]	||
		*p == Comment_Characters[ 4 ]	||
		*p == Comment_Characters[ 5 ]	||
		*p == Comment_Characters[ 6 ]	||
		*p == Comment_Characters[ 7 ]	) {
			return( 1 );
	}
	return( 0 );
}

void	Set_Comment_Characters( char * c )
{
	memset( Comment_Characters, 0, sizeof( Comment_Characters ) );
	if( c == NULL || *c == 0 ) return;
	strncpy( Comment_Characters, c, sizeof( Comment_Characters ) );
	return;
}
