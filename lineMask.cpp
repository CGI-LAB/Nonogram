#include "lineMask.h"
#include "config.h"

//This variable cannot be declared "static"
// SEGMENT[a][b] = block from a to b
LineMask_128 SEGMENT[P_SIZE + 1][P_SIZE + 1];

void initialLineMask( )
{
	for( int i = 0; i < P_SIZE + 1; i++ ){
		SEGMENT[ i ][ i ] = BLACK_SQUARE_MASK[ i ];
		for(int j = i + 1; j < P_SIZE + 1; j++){
			SEGMENT[ i ][ j ] = SEGMENT[ i ][ j - 1 ] | BLACK_SQUARE_MASK[ j ];
		}
	}
}