/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *		Writes a fixed length record to a file
 ******************************************************************************/
#include	"farlocal.h"

int	Write_Record( 	char *	filename, 
			u32	record, 
			void *	buf, 
			u32	size
	)
{
	int	FD, r;

	if( filename == 0 || *filename == 0 ) {
		Trace(	0,
			"Error Write_Record( \"%s\", %u, buf, %u ): "
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
			"Error Write_Record( \"%s\", %u, \"\", %u ): "
			"No Record!",
			filename,
			record,
			size
		);
		errno = EFAULT;
		return( -1 );
	}
	if( ( FD = File_Open( filename, O_RDWR|O_CREAT, 0666 ) ) == -1 ) {
		return( FD );
	} 

	/*
	 *	Calculate Displacement And Write The Record.
	 */

	if( record > 0 ) lseek( FD, ( record * size ), 0 );

	r = File_Write( FD, buf, size );

	File_Close( FD );
	return( r );
}
