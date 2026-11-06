/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 *
 *	Routines To Open A UDP Socket Connection
 ******************************************************************************/
#include	"farlocal.h"
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>

static	struct	hostent * GetHost( void * addr )
{
struct	hostent	*	hp;
	u32	a1, a2, a3, a4;
	uchar	addr_buf[ 4 ];

	if( ( hp = gethostbyname( addr ) ) != NULL ) return( hp );
	sscanf( (char *)addr, "%d.%d.%d.%d", &a1, &a2, &a3, &a4 );
	addr_buf[ 0 ] = (uchar)a1;
	addr_buf[ 1 ] = (uchar)a2;
	addr_buf[ 2 ] = (uchar)a3;
	addr_buf[ 3 ] = (uchar)a4;
	if( (hp = gethostbyaddr( (char *)addr_buf, 4, AF_INET )) != NULL ) {
		return( hp );
	}
#ifdef	JUNK
	FatalError( "Unknown host %s !\n", addr );	/* */
#endif
	return( NULL );
}

int	InitUDP2Host( int my_port, char * host, int port )
{
struct	sockaddr_in	me;
struct	sockaddr_in	si;
struct	hostent	*	hp;
	fard_t	*	p;
	int		fd;
	s32		e;
	s32		timeout;

	Trace(	FAR_TRC_LVL,
		"Connect2Host( %s, %d )",
		host,
		port
	);

	if( port == 0 || ( hp = GetHost( host ) ) == NULL ) {
		errno = EADDRNOTAVAIL;
		Trace(	FAR_TRC_LVL,
			"Connect2Host %s Failed: %s",
			host,
			strerror( errno )
		);
		return( -1 );
	}

	memset( &me, 0, sizeof( me ) );
	me.sin_family = AF_INET;
	me.sin_port = htons( my_port );
	me.sin_addr.s_addr = INADDR_ANY;

	memset( &si, 0, sizeof( si ) );
	si.sin_family = hp->h_addrtype;
	si.sin_port = htons( port );
	si.sin_addr.s_addr = INADDR_ANY;
	memcpy( &si.sin_addr.s_addr, hp->h_addr, hp->h_length );

	for( timeout = 1; timeout <= 8; timeout <<= 1 ) {
		fd = -1;
		if( ( fd = socket(	AF_INET,
					SOCK_DGRAM,
					0	) ) != -1 &&
		    bind(	fd,
				(void*)&me,
				sizeof( me )		) != -1 &&
		    connect(	fd,
				(struct sockaddr *)&si,
				sizeof( struct sockaddr_in ) ) != -1 ) {
			p = &File_Table[ fd ];
			p->fd    = fd;
			p->fptr  = (FILE *)0;
			p->flags = O_RDWR;
			p->mode  = FAR_SOCK | 0666;
			sprintf( p->name, "Host:%s,Port:%u", host, port );

			Trace(	FAR_TRC_LVL,
				"Connect2Host( %s, %d ) = fd:%d",
				host,
				port,
				fd
			);
			return( File_Statistics( fd ) );
		}
		e = errno;
		close( fd );
		errno = e;
		Trace(	FAR_TRC_LVL,
			"Connect2Host %s Failed(%d): %s",
			host,
			errno,
			strerror( errno )
		);
		switch( errno ) {
		case EISCONN:
		case EALREADY:
		case EINPROGRESS:
		case EINTR:
					sleep( timeout );
		case ETIMEDOUT:
					sleep( timeout );
					continue;
		case ECONNREFUSED:
		default:		break;
		}
		break;
	}
	return( -1 );
}
