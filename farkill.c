/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */

#include	<fcntl.h>
#include	<common.h>
#include	<far.h>

/*
 *	Create A Killer Script File
 */

int	CreateKillScript( char * name )
{
	int		fd;
	char		buf[ 256 ];

	if( name == 0 || *name == 0 ) {
		if( LocalName != 0 ) {
			name = LocalName;
		} else if( my_name[ 0 ] != 0 ) {
			name = my_name;
		} else {
			return( 0 );
		}
	}
	name = File_Path( buf, sizeof( buf ), "KILL_PATH", ".K%s", name );
	umask( 0 );
	fd = File_Open( name, ( O_CREAT | O_TRUNC | O_WRONLY ), 0777 );
	if( fd == -1 ) return( 0 );
	File_Write(	fd,
			buf,
			sprintf(	buf,
					"kill -15 %u\n",
					 getpid( ) 
			)
	);
	File_Close( fd );
	return( 1 );
}

/*
 *	Add To A Kill Script File
 */
int	AddToKillScript( u16 pid, char * name )
{
	char	buf[ 256 ];

	if( name == 0 || *name == 0 ) {
		if( LocalName != 0 ) {
			name = LocalName;
		} else if( my_name[ 0 ] != 0 ) {
			name = my_name;
		} else {
			return( 0 );
		}
	}
	umask( 0 );
	return(
		Add_Line(
			File_Path(
				buf,
				sizeof( buf ),
				"KILL_PATH",
				".K%s",
				name
			),
			"kill -15 %u\n",
			( pid ? pid : getpid( ) )
		) > 0 ? 1 : 0
	);
}
