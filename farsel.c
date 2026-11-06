/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	File Access Routine: Select For Read With Timeout
 *
 *	Inputs:
 *		timeout <= -1 then Routine Waits Forever For Read Data
 *		timeout  =  0 then Routine Does Not Wait For Read Data
 *		timeout  >  0 then Routine Waits Specified Milliseconds For Read Data
 *	Returns:
 *		  -2 = If timeout occurs.
 *		  -1 = If error occurs.
 *		>= 0 = If Data Available
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/select.h>

static	fd_set	read_fds;

static	void (*fd_read_funcs[ MAX_FD ])( int fd );

void	Set_FD_Read_Function( int FD, void (*func)( int fd ) )
{
	fd_read_funcs[ FD ] = func;
	if( func != NULL ) {
		FD_SET( FD, &read_fds );
	} else {
		FD_CLR( FD, &read_fds );
	}
}

int	File_Test_For_Read_Data( int FD, s32 timeout )
{
	int	i;
	struct	timeval	to;

	fd_set	fds;
	memcpy( &fds, &read_fds, sizeof( fds ) );

	memset( &to, 0, sizeof( to  ) );
	to.tv_sec  = ( timeout / 1000 );
	to.tv_usec = ( timeout % 1000 ) * 1000;

	FD_SET( FD, &fds );

	for( ; ; ) {
		switch(
			select(
				(size_t)( MAX_FD + 1 ),
				(fd_set *)&fds,
				0,
				0,
				( timeout >= 0 ? &to : 0 )
			)
		) {
		case  0:	errno = ETIME;
				return( -2 );
		case -1:	switch( errno ) {
				case EINTR:	continue;
				default:	return( -1 );
				}
		default:	break;
		}
		for( i = 0; i < MAX_FD ; i++ ) {
			if( !FD_ISSET( i, &fds ) ) continue;
			if( fd_read_funcs[ i ] != NULL ) (fd_read_funcs[ i ])( i );
		}
		if( FD_ISSET( FD, &fds ) ) return( FD );
	}
}
