#ifndef _BIT_H_
#define _BIT_H_

#include "config.h"


typedef unsigned long long int LineMask;



#define SHIFT_R(a,s) ( (LineMask)a>>((s)*2) ) 

#define SHIFT_L(a,s) ( (LineMask)a<<((s)*2) )

#define MASK50 1125899906842623ULL /*00000001111111111111111111111111111111*/


inline unsigned int count64(LineMask block){
	block -= ((block >> 1)& 0x5555555555555555ULL );
	block = ( ( block >> 2 )  & 0x3333333333333333ULL ) + (block & 0x3333333333333333ULL );
	return (( block + (block>>4)& 0x0F0F0F0F0F0F0F0FULL )* 0x0101010101010101ULL ) >> 56;
}

#define COUNT64(x) ( count64(x & MASK50) )

#endif 