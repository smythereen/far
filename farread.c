/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads A Fixed Length Record From A File
 ******************************************************************************/
#include	"farlocal.h"

int	Read_Record( 	char *	filename, 
			u32	record, 
			void *	buf, 
			u32	size
	)
{
	int	FD;
	int	r;
	int	t;

	if( filename == 0 || *filename == 0 ) {
		Trace(	0,
			"Error Read_Record( \"%s\", %u, buf, %u ): "
			"No File Name!",
			filename,
			record,
			size
		);
		errno = ENOENT;
		return( -1 );
	}
	if( size == 0 ) return( 0 );
	if( buf	 == 0 ) {
		Trace(	0,
			"Error Read_Record( \"%s\", %u, \"\", %u ): "
			"No Buffer!",
			filename,
			record,
			size
		);
		errno = EFAULT;
		return( -1 );
	}
	memset( buf, 0, size );
	FD = File_Open( filename, O_RDWR | O_CREAT, 0666 );
	if( FD == -1 ) return( FD );

	/*
	 *	Calculate Displacement And Read The Record.
	 */

	if( record > 0 ) lseek( FD, ( record * size ), 0 );

	t = 0;
	while( ( r = File_Read( FD, buf, size ) ) != size ) {
		if( r > 0 ) {
			Trace(	0,
				"Error Read_Record( \"%s\", %u, buf, %u ): "
				"Incomplete Record Read %u < %u!",
				filename,
				record,
				size,
				r,
				size
			);
			t += r;
			size -= r;
			buf = &((char *)buf)[ r ];
			continue;
		} else if ( r == 0 ) {
			continue;
		} else {
			File_Close( FD );
			return( -1 );
		}
	}
	File_Close( FD );
	return( r + t );
}
