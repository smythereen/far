/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads fixed length ASCII text line into provided buffer
 *		then chops buffer into fixed length fields and
 *		returns an ARGV style pointer to list of null
 *		terminated fields. The end
 *		of the list contains a NULL char pointer. Upon
 *		reaching an end of file the list pointer will be
 *		NULL.
 ******************************************************************************/
#include	"farlocal.h"

/* Local Function */

static	void	movechars( char ** src, char ** tgt, int len )
{
	*( *tgt -= 1 ) = 0;
	for( ; len > 0; len-- ) *( *tgt -= 1 ) = *( *src -= 1 );
}

char ** Get_Fields(	int	fields[ ],
			char *	buffer,
			u32	length
	)
{
	u32	f;
	u32	s_len;
	u32	t_len;
	u32	w_len;
	char *	s_ptr;
	char *	t_ptr;
	char **	w_ptr;

	/* Count # Of Pointers Needed */
	s_len = 0;
	t_len = 0;
	w_len = sizeof( char * );	/* Start With Null Terminator */
	for( f = 0; fields[ f ] != 0 ; f++ ) {
		s_len += fields[ f ];
		t_len += fields[ f ] + 1;
		w_len +=  sizeof( char * );
	}
	if( ( w_len + t_len ) > length ) {
		Trace(	0,
			"Error Get_Fields( #:%u, buf, %u ): "
			"Buffer size: %u is too small, minimum needed %u",
			f,
			length,
			length,
			( w_len + t_len )
		);
		return( (char **)0 );
	}
	s_ptr = (char  *)&buffer[ s_len ];
	t_ptr = (char  *)&buffer[ t_len ];
	w_ptr = (char **)&buffer[ length - w_len ];

	for( ; f > 0; f-- ) {
		movechars( &s_ptr, &t_ptr, fields[ f - 1 ] );
		w_ptr[ f - 1 ] = t_ptr;
	}
	return( w_ptr );
}

char **	Read_Fields(	char *	filename,
			int	fields[ ],
			char *	buffer,
			u32	length
	)
{
	if( Read_Line( filename, buffer, length ) == (char *)0 ) {
		return( (char **)0 );
	}
	return( Get_Fields( fields, buffer, length ) );
}
