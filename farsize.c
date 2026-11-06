/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	File Access Routine: Returns File Size Or Number Directory Entries
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/ioctl.h>
#include	<sys/stat.h>
#include	<dirent.h>

int	File_Size( int FD )
{
	int	i;
	FILE *	fd;
	char	buf[ 512 ];

	if( File_Statistics( FD ) == -1 ) return( -1 );

	switch( ( File_Table[ FD ].s.st_mode & S_IFMT ) ) {

#ifdef	WORKING_SOCKET_SIZE
	case S_IFSOCK:	/* Socket */
			i = ioctl( FD, FIONREAD, &File_Table[ FD ].s.st_size );
			if( i == -1 ) {
				Trace(	0,
					"File_Size(%d): ERROR[%d] ioctl - %s",
					FD,
					errno,
					strerror( errno )
				);
				File_Table[ FD ].s.st_size = 0;
				return( -1 );
			}
#endif

	case S_IFCHR:	/* Character Special Device */
	case S_IFREG:	/* Regular File */
	case S_IFIFO:	/* Named FIFO/Pipe */
			return( File_Table[ FD ].s.st_size );

	case S_IFDIR:	/* Directory */
			if( (fd = File_Table[ FD ].fptr) == 0 ) return( -1 );
			rewinddir( (DIR *)fd );
			errno = 0;
			for( i = 0; FILE_Gets( fd, buf, sizeof(buf) ); i++ );
			if( errno != 0 ) i = -1;
			rewinddir( (DIR *)fd );
			return( i );

	case S_IFBLK:	/* Block Special Device */
	default:	return( -1 );
	}
}

int	FILE_Size( FILE * fd )
{
	return( File_Size( FILE_To_File( fd ) ) );
}
