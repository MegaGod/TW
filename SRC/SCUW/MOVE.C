/**
*   Updated: Suttipong Kanakakorn
*            Fri  08-04-1989  20:53:38
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "..\common\cwtype.h"
#include "..\common\cscrn.h"
#include "..\common\cwgrphc.h"
#include "..\common\ekbd.h"
#include "..\common\kbdcode.h"

#include "convert.h"
#include "var.h"

#include "block.h"
#include "cw.h"
#include "del.h"
#include "edit.h"
#include "getstr.h"
#include "kbd.h"
#include "movement.h"
#include "search.h"

#include "move.h"

/****************************************************************************/
/*  Go to line specified.                                                   */
/****************************************************************************/
void goline( unsigned int linetogo ) {
	storeline( curline );
	curline = sentinel->next;
	lineno = 1;
	while ( ( lineno != linetogo ) && ( curline->next != sentinel ) ) {
		curline = curline->next;
		lineno++;
	}
	loadtoline( curline->text );
	curpage = curline;
	pagecomplete = NO;
}

/****************************************************************************/
/*  Go to column specified.(Column origin 0)                                */
/****************************************************************************/
void gocol( unsigned int coltogo, unsigned int *x ) {
	if ( coltogo > MAXCOL ) {
		workline.middle[MAXCOL + 1] = '\0';
		coltogo = MAXCOL;
	}
	if ( coltogo > ( strlen( workline.middle ) - 1 ) ) {
		coltogo = strlen( workline.middle );
	}
	if ( coltogo < firstcol ) {
		firstcol = coltogo;
		pagecomplete = NO;
	}
	*x = coltogo - firstcol;
	if ( *x >( wind.length - 2 ) ) {
		firstcol = firstcol + *x - ( wind.length - 2 );
		*x = coltogo - firstcol;
		pagecomplete = NO;
	}
	adjustcol( x ); /* adjust if cursor is positioned to end of enlarge char */
}

/****************************************************************************/
/*  Go to first column of line.                                             */
/****************************************************************************/
void home( unsigned int *x ) {
	*x = 0;
	if ( firstcol != 0 ) {
		firstcol = 0;
		pagecomplete = NO;
	}
}

/****************************************************************************/
/*  Go to end of line.                                                      */
/****************************************************************************/
void endline( unsigned int *x ) {
	gocol( strlen( workline.middle ) - 1, x );
}

/****************************************************************************/
/*  Go to top of file.                                                      */
/****************************************************************************/
void topfile( unsigned int *x ) {
	storeline( curline );
	if ( curpage != sentinel->next ) {
		curpage = sentinel->next;
		pagecomplete = NO;
	}
	curline = curpage;
	*x = 0;
	firstcol = 0;
	lineno = 1;
	loadtoline( curline->text );
}

/****************************************************************************/
/*  Go to end of file.                                                      */
/****************************************************************************/
void endfile( unsigned int *x ) {
	int count, linenum;
	storeline( curline );
	curline = sentinel->previous;
	curpage = curline;
	lineno = findlineno( curline );
	if ( pagebreak ) {
		linenum = findlineno( curpage );
	}
	count = wind.width - 1;
	while ( ( count != 0 ) && ( curpage->previous != sentinel ) ) {
		if ( pagebreak ) {
			if ( ( linenum % lineperpage ) != 0 ) {
				curpage = curpage->previous;
				linenum--;
			}
		} else {
			curpage = curpage->previous;
		}
		count--;
	}
	loadtoline( curline->text );
	endline( x );
	pagecomplete = NO;
}

void backword( unsigned int *x ) {
	int i;
	i = *x + firstcol + 1;
	if ( i != 1 ) {   /* first column ? */
		if ( ( !ISBLANK( workline.middle[i] ) ) && ( !ISBLANK( workline.middle[i - 1] ) ) ) {
			while ( ( !ISBLANK( workline.middle[i] ) ) && ( i > 0 ) ) {
				i--;
			}
		} else {
			if ( ISBLANK( workline.middle[i - 1] ) ) {
				i--;
			}
			while ( ISBLANK( workline.middle[i] ) && ( i > 0 ) ) {
				i--;
			}
			while ( !ISBLANK( workline.middle[i] ) && ( i > 0 ) ) {
				i--;
			}
		}
		gocol( i, x );
		if ( ( i == 0 ) && ( ISBLANK( workline.middle[1] ) ) ) {
			backword( x );
		}
	} else {
		if ( curline->previous != sentinel ) {
			cursor_up( );
			endline( x );
		}
	}
}

void nextword( unsigned int *x, unsigned int y ) {
	int i, j;
	i = *x + firstcol + 1;
	j = i;
	while ( !ISBLANK( workline.middle[i] ) && ( i != MAXCOL ) ) {
		i++;
	}
	while ( ISBLANK( workline.middle[i] ) && ( i != MAXCOL ) ) {
		i++;
	}
	if ( i != MAXCOL ) {
		i--;
		gocol( i, x );
	} else {
		endline( x );
		if ( ( *x + firstcol + 1 ) <= j ) {
			cursor_down( y );
			home( x );
			if ( ISBLANK( workline.middle[1] ) ) {
				nextword( x, y );
			}
		}
	}
}

void gotopage( void ) {
	char st[4];
	int linetogo;
	st[0] = '\0';
	framebox( 30 - CENTER_FACTOR, 4, 30 - CENTER_FACTOR + 29, 6, 2 );
	dispstrhgc( "��ͧ���价��˹�ҷ������� ? ", 30 - CENTER_FACTOR + 3, 5, 2 );
	if ( getstring( st, 54 - CENTER_FACTOR, 5, 3, 2, NUMBER ) ) {
		linetogo = ( atoi( st ) * lineperpage ) - lineperpage + 1;
		if ( linetogo >= 1 ) {
			goline( linetogo );
		}
	}
	pagecomplete = NO;
}

void gotoline( void ) {
	char st[5];
	register int linetogo;
	st[0] = '\0';
	framebox( 22 - CENTER_FACTOR, 4, 22 - CENTER_FACTOR + 49, 6, 2 );
	dispstrhgc( " � ��ͧ���价���÷Ѵ�������� ? (�Ѻ�ҡ�����������)", 22 - CENTER_FACTOR, 5, 2 );
	if ( getstring( st, 65 - CENTER_FACTOR, 5, 4, 2, NUMBER ) ) {
		linetogo = atoi( st );
		if ( linetogo >= 1 ) {
			goline( linetogo );
		}
	}
	pagecomplete = NO;
}

void gobeginblk( unsigned int *x ) {
	if ( haveblock( ) ) {
		goline( blkbegin.lineno );
		gocol( blkbegin.column, x );
	}
}

void goendblk( unsigned int *x ) {
	if ( haveblock( ) ) {
		goline( blkend.lineno );
		gocol( blkend.column, x );
	}
}

void quick( unsigned int *x, unsigned int *y ) {
	register int key;

	dispkey( CNTRL_Q );
	waitkbd( 3, 2 );
	key = ebioskey( 0 ) & 0xff;
	prchar( key, 0, 3, 2 );
	if ( !isalpha( key ) && !iscntrl( key ) ) {
		return;
	}
	switch ( key & 0x1f ) {
	case 'r' - 'a' + 1:
		topfile( x );
		break;
	case 'c' - 'a' + 1:
		endfile( x );
		break;
	case 'f' - 'a' + 1:
		searching( x, y );
		break;
	case 'a' - 'a' + 1:
		replacing( x, y );
		break;
	case 'y' - 'a' + 1:
		deltoendline( *x, *y );
		break;
	case 'l' - 'a' + 1:
		loadtoline( curline->text );
		refreshline( 0, *y );
		break;
	case 'b' - 'a' + 1:
		gobeginblk( x );
		break;
	case 'k' - 'a' + 1:
		goendblk( x );
		break;
	case 'd' - 'a' + 1:
		endline( x );
		break;
	case 's' - 'a' + 1:
		home( x );
		break;
	case 'x' - 'a' + 1:
		bottom_of_page( );
		break;
	case 'e' - 'a' + 1:
		top_of_page( );
		break;
	}
}
