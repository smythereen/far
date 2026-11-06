/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */

#include	"farlocal.h"

/* Local Variables */

static	int	space_flag;

static	int	isdelimiter( int c, char * d )
{
	if( c == 0 ) {
		return( 0 );
	}
	if( space_flag && isspace( c ) ) {
		return( 1 );
	}
	if( d == (char *)0 || *d == 0  ) {
		return( 0 );
	}
	while( *d != 0 ) if( c == (int)*d++ ) {
		return( 2 );
	}
	return( 0 );
}

char **	Find_Word_Values(
		char **	word,
		char **	av,
		char *	delimiters,
		char *	buffer,
		size_t	len
	)
{
	char *	p;
	char **	v;
	char **	a;
	char **	limit;

	p = delimiters;
	if( p == (char *)0 || *p == 0 ) {
		space_flag = 1;
	} else {
		space_flag = 0;
		for( ; *p != 0 ; p++ ) {
			if( !isspace( *p ) ) continue;
			space_flag = 1;
			break;
		}
	}

	memset( buffer, 0, len );
	limit = (char **)&buffer[ len - sizeof( char * ) ];
	v = (char **)&buffer[ 0 ];
	for( ; *word != NULL && v < limit; word++, v++ ) {
		if( space_flag ) *word = Trim( *word );
		if( **word == 0 ) {
			*v = (char *)limit;	// Point To A Zeroed Byte
			continue;
		}
		len = strlen( *word );
		for( a = av; *a != NULL; a++ ) {
			p = *a;
			if( space_flag ) {
				for( ; *p != 0; p++ ) {
					switch( *p ) {
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						continue;
					}
					break;
				}
			}
			if( Is_Comment( p ) ) break;
			if( strncasecmp( p, *word, len ) != 0 ) continue;
			if( !isdelimiter( p[ len ], delimiters ) ) continue;
			for( p = &p[ len ]; *p != 0 && isdelimiter( *p, delimiters ); p++ );
//			if( space_flag ) p = Trim( p );
			*v = p;
			break;
		}
		if( *a != NULL ) continue;
		*v = (char *)limit;	// Point To A Zeroed Byte
	}
	return( (char **)&buffer[ 0 ] );
}
