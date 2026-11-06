/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 *
 *	Routines To Open A Client Socket Connection
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netinet/tcp.h>
#include	<netdb.h>

static	s32	timeout;
static	int	fd;
static	struct	hostent	*	hp;
static	struct	linger		l = { 1, 1 };


static	int	Report_Error(
		char	*	dsc,
		char	*	tgt_host
	)
{
	s32	e;

	e = errno;
	if( fd >= 0 ) {
		shutdown( fd, 2 );
		close( fd );
	}
	Trace(	0,
		"Connect2Host %s: %s Failed(%d): %s",
		tgt_host,
		dsc,
		e,
		strerror( e )
	);
	switch( e ) {
	case EISCONN:
	case EALREADY:
	case EINPROGRESS:
	case EINTR:
				sleep( timeout );
				errno = e;
				return( 0 );	//	continue;
	case ETIMEDOUT:
	case ECONNREFUSED:
	default:
				errno = e;
				return( 1 );
	}
}

static	struct	hostent * GetHost( void * addr )
{
	static	char	addr_buf[ 4 ];
	static	char *	addr_list[ 4 ];
	static	struct	hostent	h;
	u32	a1, a2, a3, a4;

	if( addr == NULL ) return( NULL );
	if( !isdigit( *(char *)addr ) ) {
		if( ( hp = gethostbyname( addr ) ) != NULL ) return( hp );
	} else {
		memset( &h, 0, sizeof( h ) );
		memset( &addr_list, 0, sizeof( addr_list ) );
		h.h_addrtype = AF_INET;
		h.h_addr_list = addr_list;
		addr_list[0] = addr_buf;
		if( sscanf( (char *)addr, "%d.%d.%d.%d", &a1, &a2, &a3, &a4 ) == 4 ) {
			addr_buf[ 0 ] = (uchar)a1;
			addr_buf[ 1 ] = (uchar)a2;
			addr_buf[ 2 ] = (uchar)a3;
			addr_buf[ 3 ] = (uchar)a4;
			return( &h );
		}
	}
	errno = EADDRNOTAVAIL;
	Report_Error( "GetHost", (char *)addr );
	return( NULL );
}

int	Connect2Host( char * tgt_host, int tgt_port, int src_port )
{
struct	sockaddr_in	tgt;
struct	sockaddr_in	src;
struct	hostent	*	hp;
	int		optval;
	fard_t	*	p;

	Trace(	FAR_TRC_LVL,
		"Connect2Host( %s, %d )",
		tgt_host,
		tgt_port
	);

	fd = -1;

	if( tgt_port == 0 || ( hp = GetHost( tgt_host ) ) == NULL ) {
		return( -1 );
	}

	memset( &tgt, 0, sizeof( tgt ) );
	tgt.sin_family      = hp->h_addrtype;
	tgt.sin_port        = htons( tgt_port );
	tgt.sin_addr.s_addr = *(u32 *)hp->h_addr;

	memset( &src, 0, sizeof( src ) );
	src.sin_family = AF_INET;
	if( src_port <  0 ) {
		src.sin_port = 0;
	} else if( src_port ==  0 ) {
		src.sin_port = htons( tgt_port );
	} else {
		src.sin_port = htons( src_port );
	}
	src.sin_addr.s_addr = INADDR_ANY;

	for( timeout = 1; timeout <= 8; timeout <<= 1 ) {
		fd = -1;

		if( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
			if( Report_Error( "Socket() Creation Function", tgt_host ) ) break;
			continue;
		}

		optval = 1;
		if( setsockopt(	fd,
				SOL_SOCKET,
				SO_REUSEADDR,
				&optval,
				sizeof( optval ) ) == -1  ) {
			if( Report_Error( "Set Socket Option", tgt_host ) ) break;
			continue;
		}

		if( setsockopt(	fd,
				SOL_SOCKET,
				SO_LINGER,
				&l,
				sizeof( l ) ) == -1  ) {
			if( Report_Error( "Set Socket Linger Option", tgt_host ) ) break;
			continue;
		}

		optval = 8;
		if( setsockopt(	fd,
				SOL_SOCKET,
				TCP_MAXSEG,
				&optval,
				sizeof( optval ) ) == -1  ) {
			if( Report_Error( "Set Socket TCP_MAXSEG Option", tgt_host ) ) break;
			continue;
		}

		optval = 1;
		if( setsockopt(	fd,
				SOL_TCP,
				TCP_NODELAY,
				&optval,
				sizeof( optval ) ) == -1  ) {
			if( Report_Error( "Set Socket TCP_NODELAY Option", tgt_host ) ) break;
			continue;
		}

		if( bind( fd, (struct sockaddr *)&src, sizeof( src ) ) == -1 ) {
			if( Report_Error( "Bind() Function", tgt_host ) ) break;
			continue;
		}

		if( connect( fd, (struct sockaddr *)&tgt, sizeof( tgt ) ) == -1 ) {
			if( Report_Error( "Connect() Function", tgt_host ) ) break;
			continue;
		}

		p = &File_Table[ fd ];
		p->fd    = fd;
		p->fptr  = (FILE *)0;
		p->flags = O_RDWR;
		p->mode  = FAR_SOCK | 0666;
		sprintf( p->name, "Host:%s,Port:%u", tgt_host, tgt_port );

		Trace(	FAR_TRC_LVL,
			"Connect2Host( %s, %d ) = fd:%d",
			tgt_host,
			tgt_port,
			fd
		);

		return( File_Statistics( fd ) );
	}
	return( -1 );
}

