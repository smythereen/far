/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	File Access Routines (FAR) Global header file.
 *
 *	This file defines common structures, variables, and routines
 *	needed for far processes.
 ******************************************************************************/
#ifndef	FAR_H
#define	FAR_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<sys/stat.h>
#include	<common.h>

#define		MAX_FD		100
#define		FAR_TRC_LVL	0x40000000	/* FAR Unit's Trace Level */

/*
 *	File Access Routines - File Mode Extensions
 */

typedef	enum	farm_e	{
	FAR_FILE=S_IFREG,	/* Creates A Regular File */
	FAR_DIR =S_IFDIR,	/* Creates A Directory File */
	FAR_FIFO=S_IFIFO,	/* Creates A Named FIFO/Pipe */
	FAR_SOCK=S_IFSOCK	/* Creates A Network Socket */
}	farm_t;

typedef	struct	fard_s	{
	int	fd;			/* File Descriptor / Index */
	FILE *	fptr;			/* Stream File Descriptor */
	int	flags;			/* File Open Flags */
	int	mode;			/* File Creation Access Mode */
	char	name[ MAX_PATH ];	/* Open File's Path & Name */
	struct	stat	s;		/* Extra File Information */
}	fard_t;

/*
 *	Global File Access Routines (FAR)
 */

int	Access_Dir(	char *	path,	/* everything after last slash ignored */
			int	directory_mode
	);

int	Build_Dir(	char *	path,	/* everything after last slash ignored */
			int	directory_mode
	);

int     Delete( char * dir );

void	Close_Line(	char *	Filename
	);

#define	Close_Words	Close_Line
#define	Close_Fields	Close_Line

int	FILE_Close(	FILE *	File_Descriptor
	);

int	File_Close(	int	File_Descriptor
	);

char *	FILE_Gets(	FILE *	File_Descriptor,
			char *	Buffer,
			u32	Buffer_Length
	);

char *	File_Gets(	int	File_Descriptor,
			char *	Buffer,
			u32	Buffer_Length
	);

static	inline	__attribute__((always_inline))	int	File_Is_Directory( int FD )
{
extern	fard_t  File_Table[ MAX_FD ];
	return( S_ISDIR( File_Table[ FD ].s.st_mode ) );
}

static	inline	__attribute__((always_inline))	int	File_Is_FIFO( int FD )
{
extern	fard_t  File_Table[ MAX_FD ];
	return( S_ISFIFO( File_Table[ FD ].s.st_mode ) );
}

int	File_Link(	char *	Existing_Path,
			char *	New_Path,
			int	Force_Creation_Boolean
	);

FILE *	FILE_Open(	char *	Path,
			int	Flags,
			...			/*	int	Mode	*/
	);

int	File_Open(	char *	Path,
			int	Flags,
			...			/*	int	Mode	*/
	);

int	File_Read(	int	File_Descriptor,
			void *	Buffer,
			u32	Buffer_Length
	);

int	FILE_Size(	FILE *	File_Descriptor
	);

int	File_Size(	int	File_Descriptor
	);

static	inline	__attribute__((always_inline))	int	File_Statistics( int FD )
{
extern	fard_t  File_Table[ MAX_FD ];

	if( FD < 0 ) return( -1 );
	if( FD >= MAX_FD ) {
		Trace(	0,
			"File_Statistics(%d): Invalid File Descriptor",
			FD
		);
		return( -1 );
	}
	if( fstat( FD, &File_Table[ FD ].s ) != -1 ) return( FD );
	Trace(	0,
		"File_Statistics(%d): ERROR[%d] fstat - %s",
		FD,
		errno,
		strerror( errno )
	);
	memset( &File_Table[ FD ].s, 0, sizeof( struct stat ) );
	return( -1 );
}

int	File_Test_For_Read_Data(
			int	File_Descriptor,
			s32	timeout_in_seconds
	);

FILE *	File_To_FILE(	int	File_Descriptor
	);

int	FILE_To_File(	FILE *	File_Descriptor
	);

static	inline	__attribute__((always_inline))	int	File_Type( int FD )
{
extern	fard_t  File_Table[ MAX_FD ];
	return( ( S_IFMT & File_Table[ FD ].s.st_mode ) );
}

int	File_Write(	int	File_Descriptor,
			void *	Data,
			u32	Data_Length
	);

char ** Find_Word_Values(
			char ** word,
               		char ** av,
                	char *  delimiters,
                	char *  buffer,
                	size_t  len
        );

char **	Get_All_Lines(	int	File_Descriptor,
			char *	Buffer,
			s32	Buffer_Length
	);

char **	GET_All_Lines(	FILE *	File_Descriptor,
			char *	Buffer,
			s32	Buffer_Length
	);

char **	Get_Fields(	int	Field_Size_Array[ ],	/* Last Entry In Array = 0 */
			char *	Buffer,
			u32	Buffer_Length
	);

char **	Get_Words(	char *	Delimiters,
			char *	Buffer,
			u32	Buffer_Length
	);

int	Is_Comment(	char *	String
	);

int	Is_File_Open(	char *	File
	);

char **	Read_All_Lines(	char *	Filename,
			char *	Buffer,
			s32	Buffer_Length
	);

char **	Read_Fields(	char *	Filename,
			int	Field_Size_Array[ ],	/* Last Entry In Array = 0 */
			char *	Buffer,
			u32	Buffer_Length
	);

char *	Read_Line(	char *	Filename,
			char *	Buffer,
			u32	Buffer_Length
	);

int	Read_Record(	char *	Filename,
			u32	Record_Number,
			void *	Buffer,
			u32	Buffer_Length
	);

char **	Read_Words(	char *	Filename,
			char *	Delimiters,
			char *	Buffer,
			u32	Buffer_Length
	);

int	ScanDir(	char *	dir,
			int	l_flag,
			void	(*func)( char * dir, char * link )
	);

void    Set_Comment_Characters(
			char *	String_Of_Comment_Characters
	);

void    Set_FD_Read_Function(
			int	FD,
			void (*func)( int fd )
	);

char **	Sort_List(	char **	Null_Terminated_List
	);

int	Verify_Path(	char *	Path,
			int	Flags,
			int	Mode
	);

int	Write_Line(	char *	Filename,
			char *	Line_or_printf_format_string,
			...
	);

int	Write_LineAP(	char *	Filename,
			char *	Line_or_printf_format_string,
			va_list	variable_arg_list_pointer
	);

int	Write_Record(	char *	Filename,
			u32	Record_Number,
			void *	Record,
			u32	Record_Length
	);

/*
 *	Buffered Argument Vector Routines
 */

char **	Add_To_Vector(	char **	Argument_Vector_Pointer,
			char **	Pointer_To_Buffer_Pointer,
			s32 *	Pointer_To_Buffer_Length
	);

char **	Declare_Vector_Buffer(	char **	Pointer_To_Buffer_Pointer,
				s32 *	Pointer_To_Buffer_Length
	);

char **	Vector_Buffer(	char **	Pointer_To_Buffer_Pointer,
			s32 *	Pointer_To_Buffer_Length
	);

/*
 *	Global Socket Access Routines
 */

extern	u32	Accept_ip;		/* far IP   in network byte order */
extern	u16	Accept_port;		/* far Port in network byte order */
extern	char	Accept_Name[ 32 ];
extern	char	Accept_Port[ 6 ];

extern	int	Connect2Host(	char *	target_host,
				int	target_port,
				int	source_port	/* 0 = Same As Target Port */
	);

extern	int	Listen4Host(	int	port,
				int	mode /* 0 = 1 Connect, !0 = Multiple */
	);

extern	int	InitUDP2Host(	int	my_port,
				char *	remote_host,
				int	remote_host_port
	);

/*
 *	Kill Script Routines
 */

extern	int	CreateKillScript( char * name );

extern	int	AddToKillScript( u16 pid, char * name );

#if	defined( __cplusplus )
}
#endif

#endif
