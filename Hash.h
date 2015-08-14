#ifndef _HASH_H_
#define _HASH_H_

#include "puzzle.h"


// line solver hash.. (儲存已知的盤面)..
#if P_SIZE>=60
#define HTABLE_SIZE 1000000ULL
#else
#define HTABLE_SIZE 9999991ULL
#endif


struct hashNode{
	LineNumbers lineProblem; 
#ifdef SSE_BOARD
	LineMask_128 nowString; 
	LineMask_128 settleString;
#else
	LineMask nowString; 
	LineMask settleString;
#endif
};



void initialHash();
#ifdef SSE_BOARD
void insertHash(const LineNumbers& problem,const LineMask_128& nowString,const LineMask_128& settleString );
bool findHash(const LineNumbers& problem,const LineMask_128& nowString, LineMask_128& settleString);
#else
void insertHash(const LineNumbers& problem,const LineMask& nowString,const LineMask& settleString );
bool findHash(const LineNumbers& problem,const LineMask& nowString, LineMask& settleString);
#endif







#endif 
