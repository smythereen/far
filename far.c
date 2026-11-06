/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	Program To Verify File Access Routine (FAR) Library Functions
 ******************************************************************************/
#include	"farlocal.h"

static	void	build_dir( int ac, char *av[ ] )
{
	if( ac != 2 ) return;
	printf(	"Build Dir returned=%d\n",
		Build_Dir(
			av[ 0 ],
			strtoul( av[ 1 ], 0, 8 )
		)
	);
	exit( 0 );
}

static	void	delete( int ac, char * av[ ] )
{
	if( av[ 0 ] != NULL ){
		Delete( av[ 0 ] );
	}
	exit( 0 );
}

static	void	file_link( int ac, char *av[ ] )
{
	if( ac != 3 ) return;
	printf(	"File Link returned=%d\n",
		File_Link(
			av[ 0 ],
			av[ 1 ],
			( *av[ 2 ] == '1' ? 1 : 0 )
		)
	);
	exit( 0 );
}

static	void	file_path( int ac, char *av[ ] )
{
	char	buf[ MAX_PATH ];
	if( ac < 2 ) return;
	printf(	"File Path = \"%s\"\n",
		File_Path(	buf,
				sizeof(buf),
				av[0],
				av[1],
				av[2],
				av[3],
				av[4],
				av[5],
				av[6]
		)
	);
	exit( 0 );
}

static	void	file_size( int ac, char *av[ ] )
{
	if( ac != 1 ) return;
	printf(	"File Size = %d\n",
		File_Size( ( ac = File_Open( av[0], O_RDONLY, 666 ) ) )
	);
	File_Close( ac );
	exit( 0 );
}

static	void	get_path( int ac, char *av[ ] )
{
	char	buf[ MAX_PATH ];
	if( ac != 1 ) return;
	printf(	"Get Path = \"%s\"\n",
		Get_Path(	buf,
				sizeof(buf),
				av[0]
		)
	);
	exit( 0 );
}

static	void	read_all_lines( int ac, char *av[ ] )
{
	int	i;
	char	buf[ 1024 * 8 ];

	if( ac != 1 ) return;
	av = Read_All_Lines( av[ 0 ], buf, sizeof( buf ) );
	if( av == 0 ) {
		fprintf( stdout, "Possible Error\n" );
		exit( 1 );
	}
	for( i = 0; av[ i ] != 0; i++ ) fprintf( stdout, "%s\n", av[ i ] );
	fflush( stdout );
	fprintf( stderr, "\nTotal Lines = %d\n", i );
	fflush( stderr );
	exit( 0 );
}

static	void	read_line( int ac, char *av[ ] )
{
	char	buf[ MAX_PATH ];
	if( ac != 1 ) return;
	while( Read_Line( av[ 0 ], buf, sizeof(buf) ) != (char *)0 ) {
		fprintf( stdout, "%s", buf );
	}
	exit( 0 );
}

static	void	read_record( int ac, char *av[ ] )
{
	u32	l;
	u32	r;
	char	buf[ 2048 ];

	if( ac != 3 ) return;
	r = strtoul( av[ 1 ], 0, 0 );
	l = strtoul( av[ 2 ], 0, 0 );
	if( l > sizeof( buf ) ) l = sizeof( buf );
	if( Read_Record( av[ 0 ], r, buf, l ) != l ) return;
	fprintf( stdout, "\nRecord %u: size=%u bytes\n", r, l );
	for( r = 0; r < l; r++ ) {
		if( ( r % 25 ) == 0 ) fprintf( stdout, "\n    ", buf );
		if( isprint( buf[ r ] ) ) {
			fprintf( stdout, " %c ", buf[ r ] );
		} else {
			fprintf( stdout, "%02.2X ", (uchar)buf[ r ] );
		}
	}
	fprintf( stdout, "\n" );
	exit( 0 );
}

static	void	read_sorted( int ac, char *av[ ] )
{
	int	i;
	char	buf[ 1024 * 8 ];

	if( ac != 1 ) return;
	av = Sort_List( Read_All_Lines( av[ 0 ], buf, sizeof( buf ) ) );
	if( av == 0 ) {
		fprintf( stdout, "Possible Error\n" );
		exit( 1 );
	}
	for( i = 0; av[ i ] != 0; i++ ) fprintf( stdout, "%s\n", av[ i ] );
	fflush( stdout );
	fprintf( stderr, "\nTotal Sorted Lines = %d\n", i );
	fflush( stderr );
	exit( 0 );
}

static	void	read_words( int ac, char *av[ ] )
{
	int		l;
	char	**	w;
	char		buf[ 2048 ];

	if( ac != 1 && ac != 2 ) return;
	l = 0;
	if( ac == 2 ) {
		char *	cp;
		for( cp = av[ 1 ]; *cp != 0; cp++ ) {
			if( isspace( *cp ) ) continue;
			if( !Is_Comment( cp ) ) continue;
			Set_Comment_Characters( NULL );
			break;
		}
	}
	while((w=Read_Words(av[0],av[1],buf,sizeof(buf)))!=(char**)0) {
		fprintf( stdout, "Line %u\n", l++ );
		for( ac = 0; w[ac] != 0; ac++ ) {
			fprintf( stdout,
				 "\tword[ %u ] = \"%s\"\n",
				 ac,
				 w[ ac ] );
		}
	}
	exit( 0 );
}

static	void	scandir( int ac, char *av[] )
{
	if( ac < 1 ) return;
	if( av[ 1 ] != NULL ){
		ScanDir( av[ 0 ], atoi( av[ 1 ] ), NULL );
	} else {
		ScanDir( av[ 0 ], 0, NULL );
	}
	exit( 0 ); 
}

static	void	translate_name( int ac, char *av[ ] )
{
	if( ac != 1 ) return;
	printf(	"Translated Name = \"%s\"\n", Name_Translate( av[0] ) );
	exit( 0 );
}


static	void	values( int ac, char *av[ ] )
{
	int		l;
	char	**	w;
	char	**	value_list;
	char		buf[ 2048 ];
	char		buf2[ 2048 ];

	if( ac <= 2 ) return;
	l = 0;
	while((w=Read_Words(av[0],",",buf,sizeof(buf)))!=(char**)0) {
		fprintf( stdout, "CSV Line %u\n", l++ );
		for( ac = 0; w[ac] != 0; ac++ ) {
			fprintf( stdout,
				 "\tword[ %u ] = \"%s\"\n",
				 ac,
				 w[ ac ] );
		}
		value_list = Find_Word_Values( &av[ 2 ], w, av[ 1 ], buf2, sizeof( buf2 ) );
		fprintf( stdout, "\n" );
		for( ac = 0; av[ ac + 2 ] != 0; ac++ ) {
			fprintf( stdout,
				 "\t%-15s = \"%s\"\n",
				 av[ 2 + ac ],
				 value_list[ ac ] );
		}
	}
	exit( 0 );
}

static	void	write_line( int ac, char *av[ ] )
{
	if( ac < 2 ) return;

	Write_Line( av[0],av[1],av[2],av[3],av[4],av[5],av[6],av[7],av[8] );
	exit( 0 );
}

static	struct	list_s	{
	char *	cmd;
	void (*func)( int ac, char *av[ ] );
}	list[ ]	=	{
	{	"build_dir",		build_dir	},
	{	"delete",		delete		},
	{	"file_link",		file_link	},
	{	"file_path",		file_path	},
	{	"file_size",		file_size	},
	{	"get_path",		get_path	},
	{	"read_all",		read_all_lines	},
	{	"read_line",		read_line	},
	{	"read_record",		read_record	},
	{	"read_sorted",		read_sorted	},
	{	"read_words",		read_words	},
	{	"scan_dir",		scandir		},
	{	"translate_name",	translate_name	},
	{	"values",		values		},
	{	"write_line",		write_line	},
	{	(char *)0,		(void (*)(int, char **))0	}
};

static	void	usage( char * av[ ] )
{
	fprintf( stderr,
		 "\n\tFile Access Routines (FAR) - Verification Program\n\n"
		 "Usage:\n"
		 "\t%s\t b[uild_dir]    new_directory/ directory_mode\n"
		 "\t  \t d[elete]	file_name\n"
		 "\t  \t f[ile_link]    existing_file new_file force_boolean\n"
		 "\t  \t file_p[ath]    env_name file_fmt [...]\n"
		 "\t  \t file_s[ize]    file_name\n"
		 "\t  \t g[et_path]     environment_name\n"
		 "\t  \t r[ead_all]     file_name\n"
		 "\t  \t read_l[ine]    file_name\n"
		 "\t  \t read_r[ecord]  file_name rec_number rec_size\n"
		 "\t  \t read_s[orted]  file_name\n"
		 "\t  \t read_w[ords]   file_name [\"delimiter_list\"]\n"
		 "\t  \t s[can_dir]	directory [ show_link ]\n"
		 "\t  \t t[ranslate]    name\n"
		 "\t  \t v[alues]       file_name.csv \"delimiter_list\" FIELD1 [FIELD2 [...]]\n"
		 "\t  \t w[rite_line]   file_name write_data ...\n"
		 "\n",
		 av[ 0 ]
	);
	fflush( stderr );
	exit( 1 );
}

int	main( int ac, char * av[ ] )
{
struct	list_s	*	l;
	int		len;

	LocalName = av[ 0 ];
//	TraceLevel = FAR_TRC_LVL;
	switch( ac ) {
	default:	len = strlen( av[ 1 ] );
			for( l = list; l->cmd != (char *)0; l++ ) {
				if( strncmp( av[ 1 ], l->cmd, len ) ) continue;
				l->func( ( ac - 2 ), &av[ 2 ] );
				break;
			}
	case  2:
	case  1:
	case  0:	usage( av );
	}
	exit( 0 );
}
