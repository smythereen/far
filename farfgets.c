/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Reads variable length ASCII text line into buffer
 *		Strips/Ignores Blank & Comment Lines Also !!!!
 ******************************************************************************/
#ifdef	linux
#define		DIRENT_ILLEGAL_ACCESS	1
#endif

#include	<dirent.h>
#include	"farlocal.h"

static	char *	error_gets( FILE * fd, char * buf, u32 len, char * err )
{
	Trace(	0,
		"ERROR(%u): FILE_Gets(0x%08X,0x%08X,%u): %s",
		errno,
		fd,
		buf,
		len,
		( (err != 0 && *err != 0) ? err : strerror( errno ) )
	);
	return( (char *)0 );
}

char *	FILE_Gets( FILE * fd, char * buf, u32 len )
{
	char *	p;
	int	FD;
struct	dirent	*	d;

	if( fd == (FILE *)0 ) {
		errno = EBADF;
		return( error_gets( fd, buf, len, 0 ) );
	}
	if( buf == 0 || len == 0 ) {
		errno = EFAULT;
		return( error_gets( fd, buf, len, "No Buffer/Length!" ) );
	}

	errno = 0;
	memset( buf, 0, len );
	FD = FILE_To_File( fd );

	if( File_Is_Directory( FD ) ) {
		while( (d = readdir( (DIR *)fd ) ) != 0 ) {
			if( !strcmp(  ".", d->d_name ) ) continue;
			if( !strcmp( "..", d->d_name ) ) continue;
			sprintf(	buf,
					"%s\n",
					d->d_name
			);
			Trace(	FAR_TRC_LVL,
				"File_Gets(%d,\"%s\\n\",%d) Directory!",
				FD,
				d->d_name,
				len
			);
			return( buf );
		}
		switch( errno ) {
		default:	return( error_gets( fd, buf, len, 0 ));
		case	0:	Trace(	FAR_TRC_LVL,
					"File_Gets(%d,*EOF*,%d) Directory!",
					FD,
					len
				);
				return( (char *)0 );
		}
	}

	do {
		while( fgets( buf, len, fd ) != (char *)0 ) {
			for( p = buf; isspace( *p ); p++ );
			if( Is_Comment( p ) ) continue;
			Trace(	FAR_TRC_LVL,
				"File_Gets(%d,\"%0.*s\\n\",%d) File!",
				FD,
				( strlen( p ) - 1 ),
				p,
				len
			);
			return( p );
		}
		if( !ferror( fd ) ) continue;
		switch( errno ) {
		case EINTR:	continue;
		default:	return( error_gets( fd, buf, len, 0 ));
		}
	} while( !feof( fd ) );
	Trace(	FAR_TRC_LVL,
		"File_Gets(%d,*EOF*,%d) File!",
		FD,
		len
	);
	return( (char *)0 );
}
