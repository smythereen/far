/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Builds/Adds a string pointer to an in-buffer argument vector
 *	The vector is assumed to be at the end/back of the buffer
 *	New string argument is assumed to be at the start of the buffer
 ******************************************************************************/
#include	"farlocal.h"

char **	Vector_Buffer( char ** buffer, s32 * total )
{
	char *	b;
	s32	l;
	if( buffer == 0 || ( b = *buffer ) == 0 ) {
		Trace(	0,
			"Error Initialize_Buffer_For_Vector(0x%X,0x%X): "
			"No Buffer Pointer!",
			buffer,
			total
		);
		return( (char **)0 );
	}
	if( total == 0 || ( l = *total ) <= 0 ) {
		Trace(	0,
			"Error Initialize_Buffer_For_Vector(0x%X,0x%X): "
			"No Buffer Length/Pointer!",
			buffer,
			total
		);
		return( (char **)0 );
	}
	memset( b, 0, l );
	l -= sizeof( char * );	/* Adjust Length For Null Terminator */
	*total = l;		/* Give New Free Length Back To Caller */
	Trace(	FAR_TRC_LVL,
		"Vector_Buffer(0x%X->0x%X,0x%X->%d)=0x%X",
		buffer,
		b,
		total,
		l,
		&b[l]
	);
	return((char **)&b[l]);	/* Give Initial Vector Pointer To Caller */
}

char **	Declare_Vector_Buffer( char ** buffer, s32 * total )
{
	char *	b;
	s32	l;
	s32	d;
	if( buffer == 0 || ( b = *buffer ) == 0 ) {
		Trace(	0,
			"Error Initialize_Buffer_For_Vector(0x%X,0x%X): "
			"No Buffer Pointer!",
			buffer,
			total
		);
		return( (char **)0 );
	}
	if( total == 0 || ( l = *total ) <= 0 ) {
		Trace(	0,
			"Error Initialize_Buffer_For_Vector(0x%X,0x%X): "
			"No Buffer Length/Pointer!",
			buffer,
			total
		);
		return( (char **)0 );
	}
	d = strlen( b );
	memset( &b[ d ], 0, ( l - d ) );
	l -= sizeof( char * );	/* Adjust Length For Null Terminator */
	*total = l;		/* Give New Free Length Back To Caller */
	Trace(	FAR_TRC_LVL,
		"Declare_Vector_Buffer(0x%X->0x%X,0x%X->%d)=0x%X",
		buffer,
		b,
		total,
		l,
		&b[l]
	);
	return((char **)&b[l]);	/* Give Initial Vector Pointer To Caller */
}

char **	Add_To_Vector( char ** av, char ** buffer, s32 * total )
{
	char *	b;
	s32	l;
	s32	s;
	s32	i;

	errno = EFAULT;
	if( av == 0 ) {
		Trace(	0,
			"Error Add_To_Vector(0x%X,0x%X,0x%X): "
			"No Vector Pointer!",
			av,
			buffer,
			total
		);
		return( (char **)av );
	}
	if( buffer == 0 || ( b = *buffer ) == 0 ) {
		Trace(	0,
			"Error Add_To_Vector(0x%X,0x%X->0,0x%X): "
			"No Buffer Pointer!",
			av,
			buffer,
			total
		);
		return( (char **)av );
	}
	if( total == 0 ) {
		Trace(	0,
			"Error Add_To_Vector(0x%X,0x%X->0x%X,0x%X): "
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
			"Error Add_To_Vector(0x%X,0x%X->0x%X,0x%X->%d): "
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
			"Error Add_To_Vector(0x%X,0x%X->0x%X,0x%X->%d): "
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
			"Error Add_To_Vector(0x%X->!0,0x%X->0x%X,0x%X->%d): "
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
	l -= s;			/* Adjust Free Buffer Length For String */
	b = &b[ s ];		/* Adjust Buffer Pointer To Free Area */

	*total = l;		/* Give New Free Length Back To Caller */
	*buffer = b;		/* Give New Pointer Back To Caller */

	return( av );		/* Give New Vector Pointer Back To Caller */
}
