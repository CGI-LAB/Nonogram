#ifndef _LINESOLVER_H_
#define _LINESOLVER_H_

#include "config.h"

#include "puzzle.h"
/*
#define SOLVED 0
#define UNSOLVED 1
#define CONFLICT 2
*/
int lineSolver(const Puzzle& prob, Board& sol);


bool sprintSettle(const LineNumbers& des, LineMask inString, int i, int j, LineMask& settleString, unsigned char* Dtable = NULL);


#endif