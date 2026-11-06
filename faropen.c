/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Creates Path & Opens File For Read/Write/Append Access
 ******************************************************************************/
#ifdef		linux
#define		DIRENT_ILLEGAL_ACCESS	1
#endif

#include	<dirent.h>
#include	<sys/stat.h>
#include	"farlocal.h"

static	int	open_error( char * path, int flags, int mode, char * err )
{
	Trace(	FAR_TRC_LVL,
		"ERROR(%u): File_Open(\"%s\",0x%X,%o): %s",
		errno,
		path,
		flags,
		mode,
		( ( err != 0 && *err != 0 ) ? err : strerror( errno ) )
	);
	return( -1 );
}

static	int	odir( char * path, int flags, int mode )
{
	int		FD;
	DIR	*	dd;
	fard_t	*	p;

	while( ( dd = opendir( path ) ) == (DIR *)0 ) {
		switch( errno ) {
		default:	return( open_error( path, flags, mode, 0 ) );
		}
		break;
	} 
#ifdef		linux
	FD = dirfd(dd);
#else
	FD = dd->dd_fd;
#endif
	p = &File_Table[ FD ];
	p->fd = FD;
	p->fptr = (FILE *)dd;
	p->flags = flags;
	p->mode  = mode;
	strncpy( p->name, path, sizeof( p->name ) );
	stat( path, &p->s );
	Trace(	FAR_TRC_LVL,
		"opendir( \"%s\", 0x%X, 0%o ) = %u",
		path,
		flags,
		mode,
		FD
	);
	return( FD );
}

static	int	cdir( char * path, int flags, int mode )
{
	int	old;

	Trace( FAR_TRC_LVL, "mkdir(\"%s\",0%o)", path, mode );
	old = umask( 0 );
	while( mkdir( path, ( mode & 0777 ) ) == -1 ) {
		umask( old );
		switch( errno ) {
		default:	return( open_error( path, flags, mode, 0 ) );
		case EEXIST:	break;
		}
		break;
	}
	umask( old );
	return( odir( path, flags, mode ) );
}

static	int	ofile( char * path, int flags, int mode )
{
	int	FD;
	int	old;

	old = umask( 0 );
	while( ( FD = open( path, flags, ( mode & 0777 ) ) ) == -1 ) {
		umask( old );
		switch( errno ) {
		case 0:		break;
		case EINTR:	continue;
		default:	return(	open_error(	path,
							flags,
							( mode & 0777 ),
							0
					) );
		}
		break;
	} 
	umask( old );
	Trace(	FAR_TRC_LVL,
		"open( \"%s\", 0x%X, 0%o ) = %u",
		path,
		flags,
		mode,
		FD
	);
	return( FD );
}

static	int	cfifo( char * path, int flags, int mode )
{
	int	old;

	Trace( FAR_TRC_LVL, "mknod(\"%s\",0%o)", path, mode );
	old = umask( 0 );
	while( mknod( path, mode, 0 ) == -1 ) {
		umask( old );
		switch( errno ) {
		case EEXIST:	break;
		default:	return( open_error( path, flags, mode, 0 ) );
		}
		break;
	} 
	umask( old );
	return( ofile( path, flags, mode ) );
}

int	File_Open( char * path, int flags, ... )
{
	int		FD;
	fard_t *	p;
	struct	stat	s;
	int		mode;
	va_list		ap;

	if( ( flags & O_CREAT ) != 0 ) {
		va_start( ap, flags );
		mode = va_arg( ap, int );
		va_end( ap );
	} else {
		mode = 0666;
	}

	Trace( FAR_TRC_LVL, "File_Open(\"%s\",0x%X,0%o)", path, flags, mode );

	FD = -1;
	switch( Verify_Path( path, flags, mode ) ) {

	case  0:	/* File/Pipe/... Does NOT Exist */

		switch( ( mode & S_IFMT ) ) {

		case 0:
		case S_IFREG:	/* Create Regular File */
				FD = ofile( path, flags, mode );
				break;

		case S_IFIFO:	/* Create Named FIFO/Pipe */
				FD = cfifo( path, flags, mode );
				break;

		case S_IFDIR:	/* Create Directory */
				return( cdir( path, flags, mode ) );

#ifdef	JUNK
		case S_IFNWK:	/* Network Special Device */
#endif
		case S_IFBLK:	/* Block Special Device */
		case S_IFCHR:	/* Character Special Device */
		case S_IFSOCK:	/* Create Socket */
		default:	break;
		}
		if( FD != -1 ) break;
		return( -1 );

	default:	/* Open Existing As Regular File */
		Trace( FAR_TRC_LVL, "stat(\"%s\",0x%X)", path, &s );
		if( stat( path, &s ) == -1 ) {
			return( open_error( path, flags, mode, 0 ) );
		}
		switch( ( s.st_mode & S_IFMT ) ) {
		case S_IFDIR:	/* Create Directory */
				return( odir( path, flags, mode ) );
		default:
		case S_IFREG:	/* Open Regular File */
		case S_IFIFO:	/* Open Named FIFO/Pipe */
				FD = ofile( path, flags, mode );
				break;
		}
	case -1:
		if( FD != -1 ) break;
		return( -1 );
	}
	p = &File_Table[ FD ];
	p->fd    = FD;
	p->fptr  = (FILE *)0;
	p->flags = flags;
	p->mode  = mode;
	strncpy( p->name, path, sizeof( p->name ) );
	return( File_Statistics( FD ) );
}
