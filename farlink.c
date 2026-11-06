/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Creates A Hard Or Symbolic Link To Existing File
 ******************************************************************************/
#include	"farlocal.h"

int	File_Link( char * existing_path, char * new_path, int force )
{
		int	retval;
		int	errval;
	struct	stat	exist;
	struct	stat	new;

	switch( access( existing_path, F_OK ) ) {
	case  0:	break;
	case -1:	Trace(	0,
				"File_Link(\"%s\",\"%s\",%u) Cannot access %s - %s",
				existing_path,
				new_path,
				force,
				existing_path,
				strerror( errno )
			);
			return( 0 );	/* Failure */
	}

	retval = link( existing_path, new_path );
#ifdef	JUNK
	if( retval != -1 ) {
		if( stat( existing_path, &exist ) != -1 &&
		    stat(      new_path, &new   ) != -1 &&
		    exist.st_dev == new.st_dev &&
		    exist.st_ino == new.st_ino ) {
			return( 1 );
		}
		Trace(	0,
			"File_Link(\"%s\",\"%s\",%u) Failed Verify: %u:%u != %u:%u",
			existing_path,
			new_path,
			force,
			exist.st_dev,
			exist.st_ino,
			new.st_dev,
			new.st_ino
		);
		errno = EXDEV;
		return( 0 );
	}
#else
	if( retval != -1 ) return( 1 );
#endif
	for( ; retval == -1; ) {
		errval = errno;
		switch( errno ) {
		case EEXIST:	if( stat( existing_path, &exist ) != -1 &&
				    stat(      new_path, &new   ) != -1 &&
				    exist.st_dev == new.st_dev &&
				    exist.st_ino == new.st_ino ) {
					return( 1 );
				}
				errno = errval;
				break;

		case EACCES:	if( !Access_Dir( new_path, 0775 ) ) break;
				retval = link( existing_path, new_path );
				continue;
		case EMLINK:	/* Too Many Hard Links!  Try Symbolic */
		case EXDEV:	/* Not Same File System! Try Symbolic */
				retval = symlink( existing_path, new_path );
				continue;
		case ENOENT:	if( !force ) break;
				if( !Build_Dir( new_path, 0755 ) ) break;
				retval = link( existing_path, new_path );
				continue;
		default:	break;
		}
		Trace(	( errno == EEXIST ? FAR_TRC_LVL : 0 ),
			"File_Link(\"%s\",\"%s\",%u) Error: %s",
			existing_path,
			new_path,
			force,
			strerror( errno )
		);
		return( 0 );
	}
	return( 1 );
}
