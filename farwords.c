/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into provided buffer
 *		then chops buffer into words and returns an ARGV style
 *		pointer to list of null terminated words. The end
 *		of the list contains a NULL char pointer. Upon
 *		reaching an end of file the list pointer will be
 *		NULL.
 ******************************************************************************/
#include	"farlocal.h"

	int	No_Double_Quotes = 1;
//	int	No_Single_Quotes = 0;

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
		return( 1 );
	}
	return( 0 );
}

static	char **	Insert_Vector( char ** av, char ** buffer, s32 * total )
{
	char *	b;
	s32	l;
	s32	s;
	s32	i;

	errno = EFAULT;
	if( av == 0 ) {
		Trace(	0,
			"Error Insert_Vector(0x%X,0x%X,0x%X): "
			"No Vector Pointer!",
			av,
			buffer,
			total
		);
		return( (char **)av );
	}
	if( buffer == 0 || ( b = *buffer ) == 0 ) {
		Trace(	0,
			"Error Insert_Vector(0x%X,0x%X->0,0x%X): "
			"No Buffer Pointer!",
			av,
			buffer,
			total
		);
		return( (char **)av );
	}
	if( total == 0 ) {
		Trace(	0,
			"Error Insert_Vector(0x%X,0x%X->0x%X,0x%X): "
			"No Buffer Length Pointer!",
			av,
			buffer,
			b,
			total
		);
		return( (char **)av );
	}
	l = *total;
	if( (void *)av != (void *)&b[ l ] ) {
		Trace(	0,
			"Error Insert_Vector(0x%X,0x%X->0x%X,0x%X->%d): "
			"Invalid Vector != &Buffer[Length] !",
			av,
			buffer,
			b,
			total,
			l
		);
		return( (char **)av );
	}
	s = strlen( b ) + 1;	/* Include Null Byte In String Length */
	if( ( l - s ) < sizeof( char * ) ) {
		Trace(	0,
			"Error Insert_Vector(0x%X,0x%X->0x%X,0x%X->%d): "
			"Buffer Length Is Too Small!",
			av,
			buffer,
			b,
			total,
			l
		);
		return( (char **)av );
	}
	av = &av[ -1 ];		/* Backup Vector Pointer */
	if( *av != 0 ) {
		Trace(	0,
			"Error Insert_Vector(0x%X->!0,0x%X->0x%X,0x%X->%d): "
			"Dirty Buffer For New Vector!",
			av,
			buffer,
			b,
			total,
			l
		);
		return( (char **)&av[ 1 ] );
	}
	errno = 0;
	/*
	 *	Shift Argument Vector Down Into Buffer
	 */
	for( i = 0; ( av[i] = av[i+1] ) != 0; i++ ); /* Move Vector Down */
	/*
	 *	Add New Pointer To Vector
	 */
	av[ i ] = b;

	l -= sizeof( char * );	/* Adjust Free Buffer Length For Pointer */

	*total = l;		/* Give New Free Length Back To Caller */

	return( av );		/* Give New Vector Pointer Back To Caller */
}


char **	Get_Words(	char *	delimiters,
			char *	buffer,
			u32	length
	)
{
	int	quote;
	int	w;
	char *	p;
	char ** av;

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

	p = buffer;

	av = Declare_Vector_Buffer( &p, (s32 *)&length );

	for( w = 0; *p != 0; w++ ) {
		if( Is_Comment( p ) ) {
			*p = 0;
			return( av );
		}
		for( ; *p != 0; *p++ = 0, length-- ) {
			if( !isdelimiter( *p, delimiters ) ) {
				av = Insert_Vector( av, &p, (s32 *)&length );
				if( errno == 0 ) break;
				Trace(	0,
					"Error Get_Words( ): "
					"Reached Buffer Limit, words=%u",
					w
				);
				return( av );
			}
			if( isspace( *p ) ) continue;
			if( w == 0 ) {
				w += 1;
				av = Insert_Vector( av, &p, (s32 *)&length );
				if( errno == 0 ) continue;
				Trace(	0,
					"Error Get_Words( ): "
					"Reached Buffer Limit, words=%u",
					w
				);
				return( av );
			}
			*p++ = 0;
			length--;
			if( space_flag ) {
				while( *p != 0 && isspace( *p ) ) {
					*p++ = 0;
					length--;
				}
			}
			av = Insert_Vector( av, &p, (s32 *)&length );
			if( errno == 0 ) break;
			Trace(	0,
				"Error Get_Words( ): "
				"Reached Buffer Limit, words=%u",
				w
			);
			return( av );
		}

		if( *p == 0 ) return( av );

		av[ w ] = p;
		for( quote = 0; *p != 0; p++, length-- ) {
			if( quote ) {
				switch( *p ) {
				case '"':	quote = 0;
				default:	continue;
				case '\n':	*p = 0;
						break;
				}
				break;
			}
			if( *p == '"' ) {
				quote = 1;
				continue;
			}
			if( *p == '\n' ) {
				*p = 0;
				break;
			}
			if(  isdelimiter( *p, delimiters ) ) break;
		}
		/*
		 *	If Whole Word Is Double Quoted Then Remove Quotes
		 */
		if( No_Double_Quotes != 0 && *av[ w ] == '"' && p[ -1 ] == '"' ) {
			av[ w ] += 1;
			p[ -1 ] = 0;
		}
		if( *p == 0 ) return( av );
	}
	return( av );
}

char **	Read_Words(	char *	filename,
			char *	delimiters,
			char *	buffer,
			u32	length
	)
{
	memset( buffer, 0, length );
	if( Read_Line( filename, buffer, length ) == (char *)0 ) {
		return( (char **)0 );
	}
	return( Get_Words( delimiters, buffer, length ) );
}
