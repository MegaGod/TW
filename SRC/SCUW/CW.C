/**
*   update: Suttipong Kanakakorn
*   Sun  07-30-1989  20:11:04
*   Thu  08-03-1989  10:13:44
*   CUCC : Add more comments
*/

#include <stdio.h>
#include <stdlib.h>
#include <dir.h>

#define  DEFINE_VAR 1

#include "..\common\cwtype.h"
#include "..\common\cscrn.h"
#include "..\common\cusong.h"
#include "..\common\ekbd.h"
#include "..\common\grphc.h"
#include "..\common\kbdcode.h"
#include "..\common\sound.h"

#include "convert.h"
#include "var.h"

#undef   DEFINE_VAR

#include "..\common\cwgrphc.h"
#include "..\common\grdetect.h"

#include "block.h"
#include "cwsetup.h"
#include "del.h"
#include "edit.h"
#include "ins.h"
#include "kbd.h"
#include "menu.h"
#include "move.h"
#include "movement.h"
#include "onscrn.h"
#include "scrn.h"
#include "search.h"
#include "status.h"
#include "wrap.h"

#include "cw.h"

void dispkey( register unsigned int key ) {
	dispstrhgc( "   ", wind.col, 2, 0 );
	if ( ( key & 0xff ) < 32 ) {
		prchar( '^', 0, wind.col, 2 );
		prchar( ( key & 0xff ) + 0x40, 0, wind.col + 1, 2 );
	}
}

/****************************************************************************/
/*  Set up first set of linked list,it must consist of at least two line    */
/*  below follow.                                                           */
/*  1. sentinel line - Next pointer of sentinel node point to first line    */
/*                     & previous pointer point to last line to form        */
/*                     circular linked list.                                */
/*  2. first line - It's contain no string but it's serious required !.     */
/****************************************************************************/
void setupnode( void ) {
	sentinel = ( struct line_node * ) malloc( sizeof( struct line_node ) );
	sentinel->next = sentinel;
	sentinel->previous = sentinel;
	sentinel->text = NULL;
	sentinel->graph = NULL;
	sentinel->wrap = NO;
	curline = ( struct line_node * ) malloc( sizeof( struct line_node ) );
	curline->wrap = NO;
	curline->text = ( char * ) malloc( 1 );
	*( curline->text ) = '\0';
	curline->graph = NULL;
	insert_line( sentinel, curline );
	curpage = curline;
	loadtoline( curline->text );
}

void initscrn( void ) {
	int countcol;
	setgraph( );              /* set to graphic mode */
	clsall( );
	_rectangle( 0, 0, 639, ( scrmode == VGA ) ? 479 : ( scrmode == ATT400 ) ? 399 : 347 );
	prakeaw( );
	dispstrhgc( "����ŧ�ó�����Է�����", 6, 0, BOLDATTR );
	dispstrhgc( "� ESC<->MENU", 66, 1, BOLDATTR );
	for ( countcol = 1; countcol <= 10; countcol++ ) {
		headmenu( countcol, 0x00 );
	}
}

unsigned menu_to_key( register unsigned int curmenu ) {
	register int i;
	for ( i = 0; ( command_tab[i] != curmenu ) && ( command_tab[i] != 0 ); i += 2 );
	return( command_tab[i + 1] );
}

void main( int argc, char *argv[] ) {
	extern char prog_date[], prog_time[];
	unsigned x = 0, y = 0;		/* (x,y) position -> (0,0) at upper left on screen */
	unsigned curmenu = 0x1100;
	int i;

	cwsetup( argc, argv );

	writestatus( 0 );
	writetab( );

#ifndef EDA_VERSION

	framebox( 21 - CENTER_FACTOR, 5, ( 21 - CENTER_FACTOR ) + 45, 14, REVERSEATTR );
	dispstrhgc( "THAI  WRITER", ( 24 - CENTER_FACTOR ) + 9, 6, REVERSEATTR );
	dispstrhgc( "<to do : insert hash here>", ( 24 - CENTER_FACTOR ) + 7, 7, REVERSEATTR );
	dispprintf( ( 24 - CENTER_FACTOR ) + 9, 8, REVERSEATTR, "%11s   %8s", prog_date, prog_time );
	dispstrhgc( "Hercules/EGA/VGA/MCGA/AT&T", ( 24 - CENTER_FACTOR ) + 7, 9, REVERSEATTR );
	dispstrhgc( "Khral Steelforge", ( 24 - CENTER_FACTOR ) + 12, 10, REVERSEATTR );
	dispstrhgc( "The Forge Warband, Iron Legion", ( 24 - CENTER_FACTOR ) + 5, 11, REVERSEATTR );
	dispstrhgc( "�Ѳ�Ҩҡ ����촨��� �� ����Է����¨���ŧ�ó�", ( 24 - CENTER_FACTOR ) + 1, 13, REVERSEATTR );

	/*
	framebox( 21 - CENTER_FACTOR, 5, ( 21 - CENTER_FACTOR ) + 45, 13, REVERSEATTR );
	*/
	/*
	framebox( 21 - CENTER_FACTOR, 5, ( 21 - CENTER_FACTOR ) + 45, 14, REVERSEATTR );
	*/
	/*
	dispstrhgc( "CU  WRITER", 34 - CENTER_FACTOR, 6, REVERSEATTR );
	dispstrhgc( "Version 1.41", 37 - CENTER_FACTOR, 7, REVERSEATTR );
	dispprintf( 33 - CENTER_FACTOR, 8, REVERSEATTR, "%11s  %8s", prog_date, prog_time );
	dispstrhgc( "Hercules/EGA/VGA/MCGA/AT&T", 30 - CENTER_FACTOR, 9, REVERSEATTR );
	dispstrhgc( "�Ѳ���� ʶҺѹ��ԡ�ä��������� �����Ѻ", 27 - CENTER_FACTOR, 10, REVERSEATTR );
	dispstrhgc( "�Ҥ�Ԫ����ǡ������������� ������ǡ�����ʵ��", 25 - CENTER_FACTOR, 11, REVERSEATTR );
	*/
	/*
	dispstrhgc("��� �������������� ͺ�.", 33 - CENTER_FACTOR, 12, REVERSEATTR );
	dispstrhgc("����ŧ�ó�����Է�����", 33 - CENTER_FACTOR, 13, REVERSEATTR );
	*/
	/*
	dispstrhgc( "����ŧ�ó�����Է�����", 35 - CENTER_FACTOR, 12, REVERSEATTR );
	*/
#else

	framebox( 21 - CENTER_FACTOR, 5, ( 21 - CENTER_FACTOR ) + 45, 11, REVERSEATTR );
	dispstrhgc( "CU  WRITER", 35 - CENTER_FACTOR, 6, REVERSEATTR );
	dispstrhgc( "Version 1.2", 38 - CENTER_FACTOR, 7, REVERSEATTR );
	dispstrhgc( "EDA version", 38 - CENTER_FACTOR, 8, REVERSEATTR );
	dispstrhgc( "��Թѹ��ҡ�èҡ ����ŧ�ó�����Է�����", 28 - CENTER_FACTOR, 9, REVERSEATTR );
	dispstrhgc( "�ͺ�� ��з�ǧ��ҧ�����", 33 - CENTER_FACTOR, 10, REVERSEATTR );

#endif

	if ( cu_song ) {
		cusong( );
	}
	ebioskey( 0 );
	cls( );
	do {
		dispstrhgc( "   ", wind.col, 2, 0 );
		i = pulled_down_menu( &curmenu, &x, &y );
		if ( filename[0] != '\0' ) {
			switch ( i ) {
			case RETKEY:
				keymain = menu_to_key( curmenu );
				break;

			case ESCKEY:
				waitkbd( wind.col + x, wind.row + y );		/* Show blinking cursor */
				keymain = readkbd( );		/* If keypressed Get it */
				break;

			default:
				keymain = i;
				break;
			}

			while ( keymain != ESCKEY ) {
				if ( ( keymain & 0xff ) >= 32 ) {
					keymain = changekey( keymain );
					if ( insertmode ) {
						if ( !insert_char( keymain, &x, &y ) ) {
							linetoolong( );
						}
					} else {
						if ( !ovrwrite_char( keymain, &x, &y ) ) {
							linetoolong( );
						}
					}
					refreshline( x, y );
				} else {	/*  Function Key  */
					switch ( keymain ) {
					case PGUPKEY:
					case CNTRL_R:
						page_up( );
						break;

					case PGDNKEY:
					case CNTRL_C:
						page_down( );
						break;

					case UPKEY:
					case CNTRL_E:
						cursor_up( );
						break;

					case DNKEY:
					case CNTRL_X:
						cursor_down( y );
						break;

					case LEKEY:
					case CNTRL_S:
						cursor_left( &x );
						break;

					case 0x2301:
						gobeginblk( &x );
						break;

					case 0x2401:
						goendblk( &x );
						break;

					case RIKEY:
					case CNTRL_D:
						cursor_right( &x, y );
						break;

					case CNTRL_W:
						scroll_up( );
						break;

					case CNTRL_Z:
						scroll_down( );
						break;

					case CHOMEKEY:
						top_of_page( );
						break;

					case CPGUPKEY:
						topfile( &x );
						break;

					case CENDKEY:
						bottom_of_page( );
						break;

					case CPGDNKEY:
						endfile( &x );
						break;

					case DELKEY:
					case CNTRL_G:
						delete_char( x );
						refreshline( x, y );
						changeflag = YES;
						break;

					case CNTRL_T:
						delete_word( x );
						refreshline( x, y );
						changeflag = YES;
						break;

					case CNTRL_Y:
						delete_line( );
						changeflag = YES;
						break;

					case CNTRL_M:
					case RETKEY:
						if ( insertmode == NO ) {
							returnkey( &x, y );
						} else {
							ret_with_ins( &x, y );
							changeflag = YES;
						}
						break;

					case BSKEY:
					case CNTRL_H:
						backspace( &x );
						y = findrow( );
						refreshline( 0, y );
						changeflag = YES;
						break;

					case INSKEY:
					case CNTRL_V:
						insertmode = !insertmode;
						writeinsmode( );
						break;

					case CNTRL_N:
						insert_ret( &x );
						break;

					case F10KEY:
						thaimode = !thaimode;
						writelanguage( );
						break;

					case F1KEY:
						fontused = 0x00;
						writeattr( );
						break;

					case F3KEY:
						fontused = fontused | ONELINEATTR;
						fontused = fontused & 0x7f;
						writeattr( );
						break;

					case F4KEY:
						fontused = fontused | TWOLINEATTR;
						fontused = fontused & 0xfe;
						writeattr( );
						break;

					case F5KEY:
						fontused = fontused | BOLDATTR;
						writeattr( );
						break;

					case F6KEY:
						fontused = fontused | ENLARGEATTR;
						writeattr( );
						break;

					case F7KEY:
						fontused = fontused | SUPERATTR;
						if ( ( fontused & SUBATTR ) == SUBATTR ) {
							fontused = fontused ^ SUBATTR;
						}
						writeattr( );
						break;

					case F8KEY:
						fontused = fontused | SUBATTR;
						if ( ( fontused & SUPERATTR ) == SUPERATTR ) {
							fontused = fontused ^ SUPERATTR;
						}
						writeattr( );
						break;

					case F2KEY:
						fontused = fontused | ITALICATTR;
						writeattr( );
						break;

					case F9KEY:
						manualwrap( &x, &y );
						break;

					case ALTM:
						editmacro( );
						break;

					case TABKEY:
					case CNTRL_I:
						movetotab( &x, y );
						break;

					case CNTRL_K:
						blockcommand( &x );
						break;

					case 0x1401:
						blkcmd( 'p', &x );
						break;
					case 0x6101:
						blkcmd( 'b', &x );
						break;
					case 0x6201:
						blkcmd( 'k', &x );
						break;
					case 0x6301:
						blkcmd( 'c', &x );
						break;
					case 0x6401:
						blkcmd( 'y', &x );
						break;
					case 0x6501:
						blkcmd( 'v', &x );
						break;
					case 0x6601:
						blkcmd( 'r', &x );
						break;
					case 0x6701:
						blkcmd( 'w', &x );
						break;
					case 0x6801:
						blkcmd( 'h', &x );
						break;

					case CNTRL_O:
						onscreen( x, y );
						break;

					case 0x7101:
						doonscrn( 'l', x, y );
						break;
					case 0x7201:
						doonscrn( 'r', x, y );
						break;
					case 0x7301:
						doonscrn( 'i', x, y );
						break;
					case 0x7401:
						doonscrn( 'n', x, y );
						break;
					case 0x7501:
						doonscrn( 'c', x, y );
						break;
					case 0x7601:
						doonscrn( 'p', x, y );
						break;
					case 0x7701:
						doonscrn( 'x', x, y );
						break;

					case CNTRL_Q: quick( &x, &y );
						break;

					case 0x8111:
						inscntrl( CNTRL_W, x, y );
						break;
					case 0x8211:
						inscntrl( CNTRL_S, x, y );
						break;
					case 0x8311:
						inscntrl( CNTRL_R, x, y );
						break;
					case 0x8411:
						inscntrl( CNTRL_B, x, y );
						break;
					case 0x8511:
						inscntrl( CNTRL_E, x, y );
						break;
					case 0x8611:
						inscntrl( CNTRL_T, x, y );
						break;
					case 0x8711:
						inscntrl( CNTRL_V, x, y );
						break;

					case CNTRL_P:
						printcntrl( x, y );
						break;

					case HOMEKEY:
						home( &x );
						break;

					case ENDKEY:
						endline( &x );
						break;

					case CLEKEY:
					case CNTRL_A:
						backword( &x );
						break;

					case CRIKEY:
					case CNTRL_F:
						nextword( &x, y );
						break;

					case CNTRL_L:
						if ( source[0] != '\0' ) {
							if ( replaceflag == NO ) {
								if ( searchfwd( &x, &y ) == NO ) {
									wordnotfound( );
								}
							} else {
								if ( searchreplace( &x, &y ) == NO ) {
									wordnotfound( );
								}
							}
						}
						break;

					case CNTRL_B:
						reform( );
						break;

					case ALTP:
						gotopage( );
						break;

					case CNTRL_J:
					case ALTL:
						gotoline( );
						break;

					case 0x5101:
						searching( &x, &y );
						break;

					case 0x5201:
						replacing( &x, &y );
						break;

					case 0x8501:
						loadtoline( curline->text );
						refreshline( 0, y );
						break;

					case CF1KEY:
						insertmacro( &macro[0][0], &x, &y );
						break;
					case CF2KEY:
						insertmacro( &macro[1][0], &x, &y );
						break;
					case CF3KEY:
						insertmacro( &macro[2][0], &x, &y );
						break;
					case CF4KEY:
						insertmacro( &macro[3][0], &x, &y );
						break;
					case CF5KEY:
						insertmacro( &macro[4][0], &x, &y );
						break;
					case CF6KEY:
						insertmacro( &macro[5][0], &x, &y );
						break;
					case CF7KEY:
						insertmacro( &macro[6][0], &x, &y );
						break;
					case CF8KEY:
						insertmacro( &macro[7][0], &x, &y );
						break;
					case CF9KEY:
						insertmacro( &macro[8][0], &x, &y );
						break;
					case CF10KEY:
						insertmacro( &macro[9][0], &x, &y );
						break;

					case AF2KEY:
						inscntrl( ITALICCODE, x, y );
						break;
					case AF3KEY:
						inscntrl( ONELINECODE, x, y );
						break;
					case AF4KEY:
						inscntrl( TWOLINECODE, x, y );
						break;
					case AF5KEY:
						inscntrl( BOLDCODE, x, y );
						break;
					case AF6KEY:
						inscntrl( ENLARGECODE, x, y );
						break;
					case AF7KEY:
						inscntrl( SUPERCODE, x, y );
						break;
					case AF8KEY:
						inscntrl( SUBCODE, x, y );
						break;

					/*
					case ALTG:
						insertgraph();
						break;
					case ALTD:
						deletegraph();
						break;
					*/

					case ALTX: quitprog = YES;
						keymain = ESCKEY;
						break;

					default:
						if ( ( alt_char_map( keymain ) ) != -1 ) {
							keymain = alt_char_map( keymain );
							if ( insertmode ) {
								if ( !insert_char( keymain, &x, &y ) ) {
									linetoolong( );
								}
							} else {
								if ( !ovrwrite_char( keymain, &x, &y ) ) {
									linetoolong( );
								}
							}
							refreshline( x, y );
						}
						break;
					} /* switch ( keymain ) */
				}
				adjustcol( &x );
				while ( ( y = findrow( ) ) > ( wind.width - 1 ) ) {
					storeline( curline );
					curline = curline->previous;
					loadtoline( curline->text );
					lineno--;
				}
				if ( !keypressed( ) ) {
					if ( !pagecomplete ) {
						showpage( );
					}
					if ( !keypressed( ) ) {
						writecolno( firstcol + x );
						dispstrhgc( "   ", wind.col, 2, 0 );
						if ( !keypressed( ) ) {
							writepageline( );
						}
					}
				}
				if ( quitprog != YES ) {
					waitkbd( wind.col + x, wind.row + y );
					keymain = readkbd( );
					dispkey( keymain );
				}
			}	/* while */
		} else {	/* if filename[0] != '\0' */
			errorsound( );
		}
	} while ( !quitprog );
	if ( changeflag ) {
		blockmsg( 10 );
		dispstrhgc( "�ѧ�����Ѵ����������� ��ͧ��èѴ��������� (Y/N)?", 24 - CENTER_FACTOR, 10, 2 );
		keymain = 0;
		while ( ( keymain != 'n' ) && ( keymain != 'N' )
			&& ( keymain != 'y' ) && ( keymain != 'Y' ) ) {
			keymain = ebioskey( 0 ) & 0xff;
			if ( ( keymain == 'y' ) || ( keymain == 'Y' ) ) {
				writeblk( filename, sentinel->next, 0, sentinel->previous, MAXCOL );
			}
		}
	}
	settext( );
}