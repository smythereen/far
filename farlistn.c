/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 *
 *	Routines To Open A Server Socket Connection
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netinet/tcp.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<signal.h>

static	int	fd	= -1;
static	int	new	= -1;

	u32	Accept_ip;		/* far IP   in network byte order */
	u16	Accept_port;		/* far Port in network byte order */
	char	Accept_Name[ 32 ];
	char	Accept_Port[ 6 ];

static	int	Report_Error(
		int		port,
		char	*	dsc
	)
{
	s32	e;

	e = errno;
	if( new >= 0 ) {
		shutdown( new, 2 );
		close( new );
	}
	if( fd >= 0 ) {
		shutdown( fd, 2 );
		close( fd );
	}
	Trace(	0,
		"Listen4Host( port:%u ): %s Failed(%d): %s",
		port,
		dsc,
		e,
		strerror( e )
	);
	switch( e ) {
	case EISCONN:
	case EALREADY:
	case EINPROGRESS:
	case EINTR:
			//	sleep( timeout );
	case ETIMEDOUT:
			//	sleep( timeout );
				return( 0 );	//	continue;
	case ECONNREFUSED:
	default:		return( 1 );
	}
}

#ifdef	JUNK

char	*	GetHostName( struct sockaddr_in * sa )
{
struct	hostent	*	hp;

	if( ( hp = gethostbyaddr(	(char *)&sa->sin_addr,
					sizeof( sa->sin_addr ),
					sa->sin_family ) ) != NULL ) {
		return( (char *)hp->h_name );
	}
	return( inet_ntoa( sa->sin_addr ) );
}

#endif

int	Listen4Host( int port, int mode )
{
struct	linger		li;
struct	sockaddr_in	me;
struct	sockaddr_in	in;
	int		in_len;
	fard_t	*	p;
	int		optval;
	int		pid;

	if( port == 0 ) {
		errno = EADDRNOTAVAIL;
		return( -1 );
	}
	memset( &me, 0, sizeof( me ) );
	me.sin_family = AF_INET;
	me.sin_addr.s_addr = INADDR_ANY;
	me.sin_port = htons( port );

	fd = -1;
	new = -1;

	memset( &li, 0, sizeof( li ) );

	if((fd=	socket(	AF_INET,
			SOCK_STREAM,
			0		)	) == -1 ) {
		Report_Error( port, "Socket() Create Function" );
		return( -1 );
	}

	optval = 1;
	if( setsockopt(	fd,
			SOL_SOCKET,
			SO_REUSEADDR,
			&optval,
			sizeof( optval ) 	) == -1 ) {
		Report_Error( port, "Set Listen Socket REUSEADDR Option" );
		return( -1 );
	}

	li.l_onoff = 1;
	li.l_linger = 1;
	if( setsockopt(	fd,
			SOL_SOCKET,
			SO_LINGER,
			&li,
			sizeof( li ) 		) == -1 ) {
		Report_Error( port, "Set Listen Socket Linger Option" );
		return( -1 );
	}

	optval = 8;
	if( setsockopt(	fd,
			SOL_SOCKET,
			TCP_MAXSEG,
			&optval,
			sizeof( optval ) ) == -1  ) {
		Report_Error( port, "Set Listen Socket TCP_MAXSEG Option" );
		return( -1 );
	}

	optval = 1;
	if( setsockopt(	fd,
			SOL_TCP,
			TCP_NODELAY,
			&optval,
			sizeof( optval ) ) == -1  ) {
		Report_Error( port, "Set Listen Socket TCP_NODELAY Option" );
		return( -1 );
	}

	if( bind(	fd,
			(void*)&me,
			sizeof( me )		) == -1 ) {
		Report_Error( port, "Bind() Function" );
		return( -1 );
	}

	if( listen(	fd,
			(mode ? SOMAXCONN : 1)	) == -1 ) {
		Report_Error( port, "Listen() Function" );
		return( -1 );
	}

	for( ; ; ) {
		in_len = sizeof( in );
		memset( &in, 0, in_len );

		new = -1;
		if( ( new = accept(	fd,
					(void*)&in,
					(socklen_t *)&in_len	) )	== -1 ) {
			Report_Error( port, "Accept() Function" );
			return( -1 );
		}

		if( mode ) {
			New_Err_File( );	// Adjust Trace File Name On New Day
			signal( SIGCLD, SIG_IGN );	/* No Zombies */
		}

		strncpy(
			Accept_Name,
			inet_ntoa( in.sin_addr ),
			sizeof( Accept_Name )
		);
		Accept_ip = in.sin_addr.s_addr;

		sprintf(
			Accept_Port,
			"%u",
			ntohs( in.sin_port )
		);
		Accept_port = in.sin_port;

		optval = 1;
		if( setsockopt(	new,
				SOL_SOCKET,
				SO_REUSEADDR,
				&optval,
				sizeof( optval ) ) == -1  ) {
			Report_Error( port, "Set Accept Socket REUSEADDR Option" );
			return( -1 );
		}

		li.l_onoff = 1;
		li.l_linger = 1;
		if( setsockopt(	new,
				SOL_SOCKET,
				SO_LINGER,
				&li,
				sizeof( li ) ) == -1  ) {
			Report_Error( port, "Set Accept Socket Linger Option" );
			return( -1 );
		}

		optval = 8;
		if( setsockopt(	new,
				SOL_SOCKET,
				TCP_MAXSEG,
				&optval,
				sizeof( optval ) ) == -1  ) {
			Report_Error( port, "Set Accept Socket TCP_MAXSEG Option" );
			return( -1 );
		}

		optval = 1;
		if( setsockopt(	new,
				SOL_TCP,
				TCP_NODELAY,
				&optval,
				sizeof( optval ) ) == -1  ) {
			Report_Error( port, "Set Accept Socket TCP_NODELAY Option" );
			return( -1 );
		}

		if( mode ) {
			pid = fork( );
		} else if ((pid = shutdown( fd, 2 )) == -1 ) {
			pid = 0;
		}

		switch( pid ) {
		case -2:	p = &File_Table[ fd ];
				p->fd    = fd;
				p->fptr  = (FILE *)0;
				p->flags = O_RDWR;
				p->mode  = FAR_SOCK | 0666;
				sprintf( p->name, "Port:%u", port );
				return( File_Statistics( fd ) );

		case -1:
				Report_Error( port, "Fork() Function" );
				return( -1 );

		case 0:		close( fd );
				fd = dup2( new, fd );
				close( new );
				if ( fd == -1 ) return( fd );
				p = &File_Table[ fd ];
				p->fd    = fd;
				p->fptr  = (FILE *)0;
				p->flags = O_RDWR;
				p->mode  = FAR_SOCK | 0666;
				sprintf( p->name, "Port:%u", port );
				return( File_Statistics( fd ) );

		default:
//				Trace( 0, "Spawned Child Pid = %d", pid );
				close( new );
				break;
		}
	}
}
