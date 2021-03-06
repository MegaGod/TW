#include "cwtype.h"
#include "ekbd.h"
#include "sound.h"

#include "cusong.h"

static int cusong_data[] = {
	CC, 300, A, 300, CC, 300, E, 300, A, 300, G, 300, G, 1800,
	CC, 300, A, 300, E, 300, A, 300, G, 300, D, 300, D, 1800,
	E, 300, D, 300, E, 300, Aa, 600, C, 600, D, 1800,
	C, 600, D, 900, E, 300, G, 1800,

	CC, 300, A, 300, CC, 300, E, 300, A, 300, G, 300, G, 1800,
	CC, 300, A, 300, E, 300, A, 300, G, 300, D, 300, D, 1800,
	G, 300, E, 300, G, 300, D, 600, E, 600, G, 1500,
	Gg, 600, Aa, 900, D, 300, C, 1800,

	A, 600, G, 900, A, 300, E, 1800,
	A, 600, G, 900, E, 300, D, 1800,
	E, 300, D, 300, E, 300, Aa, 600, C, 600, D, 1800,
	E, 300, D, 300, E, 300, Gg, 600, Aa, 600, C, 1800,

	CC, 300, A, 300, CC, 300, E, 300, A, 300, G, 300, G, 1800,
	CC, 300, A, 300, E, 300, A, 300, G, 300, D, 300, D, 1800,
	G, 300, E, 300, G, 300, D, 600, E, 600, G, 1500,
	Gg, 600, Aa, 900, D, 300, C, 1800,

	A, 600, G, 900, A, 300, E, 1800,
	A, 600, G, 900, E, 300, D, 1800,
	E, 300, D, 300, E, 300, Aa, 600, C, 600, D, 1800,
	E, 300, D, 300, E, 300, Gg, 600, Aa, 600, C, 1800,

	CC, 300, A, 300, CC, 300, E, 300, A, 300, G, 300, G, 1800,
	CC, 300, A, 300, E, 300, A, 300, G, 300, D, 300, D, 1800,
	G, 300, E, 300, G, 300, D, 600, E, 600, G, 1500,
	G, 600, A, 900, CC, 300, CC, 1800,
};

void cusong( void ) {
	register int i;
	for ( i = 0; !keypressed( ) && i < sizeof( cusong_data ) / sizeof( int ); i += 2 ) {
		soundnoise( cusong_data[i], cusong_data[i + 1] );
	}
}
