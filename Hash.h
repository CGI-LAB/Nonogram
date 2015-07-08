#ifndef _HASH_H_
#define _HASH_H_

#include "puzzle.h"



#define HTABLE_SIZE 9999991ULL


struct hashNode{
	LineNumbers lineProblem; LineMask nowString; LineMask settleString;
};



void initialHash();

void insertHash(const LineNumbers& problem,const LineMask& nowString,const LineMask& settleString );

bool findHash(const LineNumbers& problem,const LineMask& nowString, LineMask& settleString);







#endif 