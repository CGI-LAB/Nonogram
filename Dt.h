#ifndef _DT_H_
#define _DT_H_
#include "puzzle.h"
#include "LineSolver.h"
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>
#include <sstream>

void UglyDt(const Puzzle problem, const Board solution) ;
bool CheckMaxFlow(long idealFlow, std::stringstream& sStream) ;
bool IsLegal( const Puzzle problem, const Board solution) ;
bool FindNode( bool aMatrix[S_SIZE * 2][S_SIZE * 2], int nSource, int nGoal) ;
int Solve2SatOrDT( const Puzzle problem, Board& solution ) ;
bool SolveDt( const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] ) ;

void MakeDt(const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] ) ;
void Find2SATRelations(const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] ) ;


#endif
