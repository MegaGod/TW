/****************************************************************************/
/*  GETSTR.C 15 JAN 89                                                      */
/*  Updated: Suttipong Kanakakorn                                           */
/*          Wed  08-02-1989  09:52:44                                       */
/****************************************************************************/

#include <string.h>

#include "..\common\cwtype.h"
#include "..\common\cscrn.h"
#include "..\common\ekbd.h"
#include "..\common\grphc.h"
#include "..\common\kbdcode.h"
#include "..\common\sound.h"

#include "const.h"
#include "var.h"

#include "kbd.h"
#include "status.h"
#include "tutil1.h"

#include "getstr.h"

/*****************************************************************************/
/* get string                                                                */
/* input                                                                     */
/*      textst : string want to get                                          */
/*      x      : vertical position ( 0 - 89 )                                */
/*      y      : horisontal position ( 0 - 16 )                              */
/*      maxlen : maximum length of string                                    */
/*      attr   : attribute of string for displaying                          */
/*      mode   : THAIENG,ENGLISH,ENGUPCASE,NUMBER,ONEORTWO                   */
/*****************************************************************************/
int getstring( char textst[], unsigned int x, unsigned int y,
	unsigned int maxlen, char attr, strtype mode ) {
	int inkey, key, oldlen, temp;
	char keepchar;

	keepchar = '\0';
	oldlen = strlen( textst );
	dispblank( x, y, maxlen, attr );
	dispstrhgc( textst, x, y, attr );
	waitkbd( x + thaistrlen( textst ), y );
	switch ( mode ) {
	case THAIENG: inkey = readkbd( );
		break;
	case NUMBER:
	case ONEORTWO:
	case ENGLISH: inkey = ebioskey( 0 );
		break;
	case ENGUPCASE:
		inkey = ebioskey( 0 );
		key = inkey & 0xff;
		if ( ( key >= 'a' ) && ( key <= 'z' ) )
			inkey = key - ( 'a' - 'A' );
		break;
	}
	switch ( inkey ) {
	case CNTRL_H:
	case BSKEY:
	case CNTRL_S:
	case LEKEY:
	case CNTRL_M:
	case RETKEY: break;
	default:
		key = inkey & 0xff;
		if ( mode == NUMBER ) {
			if ( ( key < '0' ) || ( key > '9' ) ) break;
		}
		if ( mode == ONEORTWO ) {
			if ( ( key != '1' ) && ( key != '2' ) ) break;
		}
		if ( ( inkey & 0xff ) >= 32 ) {
			textst[0] = '\0';
			dispblank( x, y, maxlen, attr );
		}
		break;
	}
	do {
		switch ( inkey ) {
		case CNTRL_M:
		case RETKEY: return( YES );
		case CNTRL_U: return( NO );              /* Abort */
		case ESCKEY: return( ESCKEY );
		case UPKEY: return( UPKEY );
		case DNKEY: return( DNKEY );
		case CNTRL_H:
		case BSKEY:
		case LEKEY:
		case CNTRL_S:
			temp = strlen( textst );
			if ( temp != 0 ) {
				if ( temp < oldlen )
					textst[temp] = keepchar;
				keepchar = textst[temp - 1];
				textst[temp - 1] = '\0';
				dispblank( x, y, maxlen, attr );
				dispstrhgc( textst, x, y, attr );
			}
			break;
		case RIKEY:
		case CNTRL_D:
			temp = strlen( textst );
			if ( ( temp < oldlen ) && ( temp < maxlen ) ) {
				textst[temp] = keepchar;
				keepchar = textst[temp + 1];
				textst[temp + 1] = '\0';
				dispblank( x, y, maxlen, attr );
				dispstrhgc( textst, x, y, attr );
			}
			break;
		case CNTRL_Y:
			if ( strlen( textst ) < oldlen )
				textst[strlen( textst )] = keepchar;
			keepchar = textst[0];
			textst[0] = '\0';
			dispblank( x, y, maxlen, attr );
			break;
		case CNTRL_R:
			if ( strlen( textst ) < oldlen ) {
				textst[strlen( textst )] = keepchar;
				dispblank( x, y, maxlen, attr );
				dispstrhgc( textst, x, y, attr );
			}
			break;
		case F10KEY:
			thaimode = !thaimode;
			writelanguage( );
			break;
		default:
			inkey = ( inkey & 0xff );
			if ( mode == NUMBER ) {
				if ( ( inkey < '0' ) || ( inkey > '9' ) ) break;
			}
			if ( mode == ONEORTWO ) {
				if ( ( inkey != '1' ) && ( inkey != '2' ) ) break;
			}
			if ( inkey >= 32 && strlen( textst ) < maxlen ) {
				if ( whatlevel( inkey ) == MIDDLE ) {
					textst[strlen( textst ) + 1] = '\0';
					textst[strlen( textst )] = inkey;
					oldlen = strlen( textst );
					dispblank( x, y, maxlen, attr );
					dispstrhgc( textst, x, y, attr );
				} else {
					if ( ( strlen( textst ) != 0 ) &&
						( whatlevel( inkey ) > whatlevel( textst[strlen( textst ) - 1] ) ) &&
						( inkey != HUNAKADMITO ) ) {
						textst[strlen( textst ) + 1] = '\0';
						textst[strlen( textst )] = inkey;
						oldlen = strlen( textst );
						dispblank( x, y, maxlen, attr );
						dispstrhgc( textst, x, y, attr );
					} else {
						errorsound( );
					}
				}
			}
			break;
		}
		waitkbd( x + thaistrlen( textst ), y );
		switch ( mode ) {
		case THAIENG:
			inkey = readkbd( );
			break;
		case NUMBER:
		case ONEORTWO:
		case ENGLISH:
			inkey = ebioskey( 0 );
			break;
		case ENGUPCASE:
			inkey = ebioskey( 0 );
			key = inkey & 0xff;
			if ( ( key >= 'a' ) && ( key <= 'z' ) )
				inkey = key - ( 'a' - 'A' );
			break;
		}
	} while ( 1 );
}

int getname( char textst[], unsigned int x, unsigned int y, unsigned int maxlen, char attr ) {
	int i;

	char drv[MAXDRIVE + 1], dir[MAXDIR + 1], name[MAXFILE + 1], ext[MAXEXT + 1];
	char textst2[80];
	do {
		i = getstring( textst, x, y, maxlen, attr, ENGUPCASE );
	} while ( ( i != YES ) && ( i != ESCKEY ) && ( i != NO ) );
	if ( i == YES ) {
		fnsplit( textst, drv, dir, name, ext );
		fnmerge( textst2, drv, dir, name, ext );
		if ( strcmp( textst, textst2 ) != 0 ) {
			errorsound( );
			blockmsg( 10 );
			dispstrhgc( "������������żԴ��Ҵ ! �� <ESC> ���ͷӧҹ���", 27, 10, REVERSEATTR );
			while ( ebioskey( 0 ) != ESCKEY );
			return( ESCKEY );
		}
	}
	return( i );
}
