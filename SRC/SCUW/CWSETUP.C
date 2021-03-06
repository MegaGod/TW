/**
*   Written: Suttipong Kanakakorn
*   Mon  07-31-1989  20:31:09
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <conio.h>

#include "..\common\cwtype.h"
#include "..\common\cscrn.h"
#include "..\common\cwgrphc.h"
#include "..\common\cwtype.h"
#include "..\common\ekbd.h"
#include "..\common\fileutil.h"
#include "..\common\grdetect.h"
#include "..\common\grphc.h"
#include "..\common\kbdcode.h"

#include "convert.h"
#include "var.h"

#include "cw.h"
#include "edit.h"
#include "file.h"
#include "getstr.h"
#include "tutil1.h"

#include "cwsetup.h"

void set_option_to_default( void );

extern int herc_align; /* from hdisp.asm */
static char cup_option[16]; /* assume that only 16 char max for argument */

void cwsetup( int argc, char *argv[] ) {
	unsigned int dummy;
	register int i;

	progname = argv[0];

	while ( ( --argc > 0 ) && ( ( i = ( *++argv )[0] ) == '/' || i == '-' ) ) {
		strcat( cup_option, argv[0] );
		strcat( cup_option, " " );
		strupr( ++argv[0] );
		while ( i = *argv[0]++ )
			switch ( i ) {
			case 'H':		/* e alone = ega, em = ega monochrome */
				scrmode = HERCMONO;
				break;
			case 'E':		/* m alone = mcga */
				scrmode = EGA;
				break;
			case 'M':
				if ( scrmode == EGA ) {
					scrmode = EGAMONO;
				} else {
					scrmode = MCGA;
				}
				break;
			case 'V':
				scrmode = VGA;
				break;
			case 'A':
				scrmode = ATT400;
				break;
			case 'L':		/* /L for Hercules,  left-justified  */
				herc_align = 0;
				break;
			case 'N':
				create_bak = NO;
				break;
			case 'W':
				wordwrap = NO;
				break;
			case 'P':
				pagebreak = NO;
				break;
			case 'S':
				cu_song = YES;
				break;
			default:  usage( );
		}
	}
	if ( cup_option[0] == '\0' ) {
		strcpy( cup_option, "/" );
	}

	if ( argc >= 1 ) {
		placekey( ' ' );
		if ( file_exist( argv[0] ) ||
			strchr( argv[0], '*' ) ||
			strchr( argv[0], '?' ) ) {
			placekey( DNKEY );
		}
		placekey( RETKEY );
		while ( i = *argv[0]++ ) {
			placekey( i );
		}
		placekey( RETKEY );
	}

	set_directory( );
	if ( readoption( AUTO_FIND ) == 0 ) {
		set_option_to_default( );
	}
	readscrfont( dfont, "NORMAL.FON", cw_dir );
	readscrfont( ditalicfont, "ITALIC.FON", cw_dir );

	harderr( handler );
	initvalue( &dummy, &dummy );
	for ( i = 0; i < MAXCOL; ++i ) {
		tab[i] = NO;
	}
	for ( i = 5; i < ( rightmar - 6 ); i += 5 ) {
		tab[i] = YES;
	}
	for ( i = 0; i <= 10; i++ ) {
		macro[i][0] = '\0';
	}
	filename[0] = '\0';

	initscrn( );

	wind.row = 4;		/* set window of edit area */
	wind.col = 1;
	wind.length = 78;
	wind.width = ( scrmode == VGA ) ? 19 : ( scrmode == ATT400 ) ? 15 : 13;
	offbreak( );		/* Turn off Ctrl-C checking */
}

void usage( void ) {
	fputs( "Usage: TW.EXE [option] [file]\n", stderr );
	fputs( "Options\n", stderr );
	fputs( "\t-h,\t/h  Hercules graphic adapter\n", stderr );
	fputs( "\t-hl,\t/hl Hercules graphic adapter, left justify\n", stderr );
	fputs( "\t-v,\t/v  vga\n", stderr );
	fputs( "\t-e,\t/e  ega with ega display\n", stderr );
	fputs( "\t-em,\t/em ega with monochrome display\n", stderr );
	fputs( "\t-m,\t/m  mcga\n", stderr );
	fputs( "\t-a,\t/a  at&t400\n", stderr );
	fputs( "\t-n,\t/n  no backup file created\n", stderr );
	fputs( "\t-w,\t/w  no word wrap\n", stderr );
	fputs( "\t-p,\t/p  no page break\n", stderr );
	exit( 1 );
}

void set_directory( void ) {
	char drive[MAXDRIVE], dir[MAXDIR], name[MAXFILE], ext[MAXEXT];

	/* build cw_dir like this -> c:\edit\cuwriter */
	fnsplit( progname, drive, dir, name, ext );
	sprintf( cw_dir, "%s%s", drive, dir );
	cw_dir[strlen( cw_dir ) - 1] = '\0'; /* clear \ */
}

typedef struct {
	char *option_name;
	int *p_option_value;
	int p_option_default;
	unsigned char maxlen;
	strtype stype;
} each_option_setup;

each_option_setup option_setup[] = {
	{ "���� (1.���. 2.�ɵ�)", ( int * ) &stdcode, YES, 1, ONEORTWO },
	{ "��˹�� (1.�� 2.�����)", ( int * ) &pagebreak, YES, 1, ONEORTWO },
	{ "�Ѵ���ѵ��ѵ� (1.�Ѵ 2.���Ѵ)", ( int * ) &wordwrap, YES, 1, ONEORTWO },
	{ "���ҧ������ͧ�ء���駷���ա���红����� (1.���ҧ 2.������ҧ)", ( int * ) &create_bak, YES, 1, ONEORTWO },
	{ "���ŧ (1. �� 2. �����) ", ( int * ) &cu_song, YES, 1, ONEORTWO },
	{ "���˹�ҫ���", &leftmar, 1, 3, NUMBER },
	{ "���˹�Ң�� (����Թ 256)", &rightmar, 65, 3, NUMBER },
	{ "�ӹǹ��÷Ѵ���˹��", &lineperpage, 33, 3, NUMBER },
};

/* number of element of option_setup */
#define NELEM_OPTION ( sizeof( option_setup ) / sizeof( option_setup[0] ) )

/*  read option from file cw.cfg
mode == AUTO try reading from current directoy first, then cw_dir
if not found use the default value
mode == CUR_DIR read from current dir
if not found display error
mode == CW_DIR  read from cw.exe directory
if not found display error
*/
int readoption( search_file_mode mode ) {
	FILE *fp;
	char fname[MAXPATH];
	int  field, temp;
	each_option_setup *op;


	sprintf( fname, "%s\\TW.CFG", cw_dir );
	if ( mode == CUR_DIR || mode == AUTO_FIND ) {
		fp = fopen( "TW.CFG", "rt" );
	} else {
		fp = fopen( fname, "rt" );
	}
	if ( fp == NULL && mode == AUTO_FIND ) {
		/* fopen file not succes */
		/* continue searching in cw_dir */
		if ( ( fp = fopen( fname, "rt" ) ) == NULL )
			return 0; /* if not found in both dir return 0 for autofind */
	}
	if ( fp == NULL ) {
		showerrno( );
		/* if search in CUR_DIR or CW_DIR and not found return -1 */
		return -1;
	}
	/* If we reach here we succesfully open cw.cfg file */
	for ( op = option_setup; op < option_setup + NELEM_OPTION; op++ ) {
		field = fscanf( fp, "%d", &temp );
		if ( field == 1 ) { /* succesfully scan */
			if ( *op->p_option_value == -1 ) /* not set from command line */
				/* so set it */
				*op->p_option_value = temp;
		} else { /* error */
			break;
		}
	}
	fclose( fp );
	return 1;
}

/* save option to file cw.cfg in either current directory or cw_dir */
void saveoption( search_file_mode mode ) {
	FILE *fp;
	char config_file[MAXPATH];
	each_option_setup *op;

	if ( mode == CUR_DIR || mode == AUTO_FIND ) {
		fp = fopen( "TW.CFG", "wt" );
	} else {
		sprintf( config_file, "%s\\TW.CFG", cw_dir );
		fp = fopen( config_file, "wt" );
	}
	if ( fp == NULL ) {
		return;
	}
	for ( op = option_setup; op < option_setup + NELEM_OPTION; op++ ) {
		fprintf( fp, "%d ", *op->p_option_value );
	}
	fprintf( fp, "\n" );
	fclose( fp );
}

void dispoption( int i ) {

	/* dispstrhgc("     ", 65 - CENTER_FACTOR + 2, i + 6, 0); */
	dispstrhgc( option_setup[i].option_name,
		65 - CENTER_FACTOR - thaistrlen( option_setup[i].option_name ),
		i + 6, 0 );
	dispprintf( 65 - CENTER_FACTOR, i + 6, 0, " :%d",
		*option_setup[i].p_option_value );
}

void setoption( void ) {
	int i, c;
	char st[5];

	pagecomplete = NO;
	if ( stdcode == 0 ) { stdcode = 2; }
	if ( pagebreak == 0 ) { pagebreak = 2; }
	if ( wordwrap == 0 ) { wordwrap = 2; }
	if ( create_bak == 0 ) { create_bak = 2; }
	if ( cu_song == 0 ) { cu_song = 2; }
	framebox( 14 - CENTER_FACTOR, 5, 14 - CENTER_FACTOR + 67, 5 + 1 + NELEM_OPTION, 0 );
	for ( i = 0; i < NELEM_OPTION; i++ ) {
		dispoption( i );
	}
	i = 0;
	do {
		itoa( *option_setup[i].p_option_value, st, 10 );
		c = getstring( st, 65 + 2 - CENTER_FACTOR, 6 + i, option_setup[i].maxlen,
			0, option_setup[i].stype );
		*option_setup[i].p_option_value = atoi( st );

		switch ( c ) {
		case UPKEY:
			dispoption( i );
			if ( i == 0 ) {
				i = NELEM_OPTION - 1;
			} else {
				i--;
			}
			break;
		case DNKEY:
			dispoption( i );
			if ( i == NELEM_OPTION - 1 ) {
				i = 0;
			} else {
				i++;
			}
			break;
		case YES:
			dispoption( i );
			if ( i < NELEM_OPTION - 1 ) {
				i++;
				break;
			}
		case ESCKEY:
			if ( stdcode == 2 ) { stdcode = 0; }
			if ( pagebreak == 2 ) { pagebreak = 0; }
			if ( wordwrap == 2 ) { wordwrap = 0; }
			if ( create_bak == 2 ) { create_bak = 0; }
			if ( cu_song == 2 ) { cu_song = 0; }
			return;
		}
	} while ( 1 );
}

void set_option_to_default( void ) {
	each_option_setup *op;

	for ( op = option_setup; op < option_setup + NELEM_OPTION; op++ ) {
		/* not set from command line, or from config file */
		if ( *op->p_option_value == -1 ) {
			/* so set it */
			*op->p_option_value = op->p_option_default;
		}
	}
}

void initvalue( unsigned *x, unsigned *y ) {
	lineno = 1;
	*x = 0;
	*y = 0;
	firstcol = 0;
	blkbegin.lineno = 1;
	blkend.lineno = 1;
	blkbegin.column = 0;
	blkend.column = 0;
	dispblock = YES;
}

void print_file( void ) {
	static char cup[] = "TWPRINT.EXE";
	char ncup[40];

	savepic( );
	if ( changeflag ) {
		framebox( 4, 3, 4 + 2 + 37 + thaistrlen( filename ) + 2, 10, NORMALATTR );
		dispprintf( 4 + ( 2 + 37 + thaistrlen( filename ) + 2 - 13 ) / 2, 4, BOLDATTR,
			"**�ô�ѧࡵ�**" );
		dispprintf( 4 + 2, 5, NORMALATTR,
			"TWPRINT �о��������Ũҡ��� 㹴�ʡ���ҹ��" );
		dispprintf( 4 + 2, 6, NORMALATTR,
			"�����  %.30s  㹴�ʡ����ç�Ѻ�������˹��¤�����", filename );
		dispprintf( 4 + 2, 7, NORMALATTR,
			"�ѧ��鹼š�þ��������ç�Ѻ�������˹��¤�����" );
		dispprintf( 4 + ( 2 + 37 + thaistrlen( filename ) + 2 - 17 ) / 2, 9, NORMALATTR,
			"�����������ͷӧҹ���" );
		ebioskey( 0 );
	}
	settext( );
	/* Find CUPRINT by PATH */
	if ( spawnlp( P_WAIT, cup, cup, cup_option, filename, NULL ) == -1 ) {
		strcpy( ncup, cw_dir );                         /* Not found */
		strcat( ncup, "\\" );                           /* Search at CW dir */
		strcat( ncup, cup );
		if ( spawnl( P_WAIT, ncup, ncup, cup_option, filename, NULL ) == -1 ) {
			printf( "\n\nCannot find file : TWPRINT.EXE\nPress any key\n\007" );
			getch( );
		}
	}
	initscrn( );
	retpic( );
	showpageall( );
}

void edit_font( void ) {
	static char cuf[] = "TWFONT.EXE";

	if ( spawnlp( P_WAIT, cuf, cuf, cup_option, NULL ) == -1 )
		showerrno( );
	initscrn( );
	showpageall( );
}

void offbreak( ) {
	union REGS inreg, outreg;

	inreg.x.ax = 0x3301;
	inreg.h.dl = 0;
	intdos( &inreg, &outreg );
}
