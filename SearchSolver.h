#ifndef _SEARCHSOLVER_H_
#define _SEARCHSOLVER_H_

#include "puzzle.h"
#include <algorithm>
#include <queue>
#include "Dependency.h"
using namespace std ;



struct SquareToGo
{
  Board firstGo ;
  Board secondGo ;
} ;

struct SearchStack
{
    int step[STACK_MAX_DEPTH];
    int state;  
    int depth;
    int nodeLimit;
    int nodeCount;
    Board solution; 
    struct SquareToGo whereCanIgo[STACK_MAX_DEPTH];
 //   struct DependencyTable;
    inline void initial()
    {
        
        memset(step,0,sizeof(step));
        initBoard(solution);
        depth = 0;
        nodeCount = 0;
        state = UNSOLVED;
    }
    inline void save(const char* filename)
    {
        FILE* fp = fopen(filename, "wb");
        fwrite(this, 1, sizeof(*this), fp);
        fclose( fp );
    }
    inline bool load(const char* filename)
    {
        FILE* fp = fopen(filename, "rb");
        if(fp == NULL) 
            return false;
        fread(this, 1, sizeof(*this), fp);
        fclose( fp );
        return true;
    }
} ;

void writeResult() ;

inline int solveOneTwoSat( const Puzzle& problem,
                           Board& correctBoard, 
                           Board& ifGuessBlackBoard, 
                           Board& ifGuessWhiteBoard, 
                           int x, 
                           int y 
                         ) ;

inline int solveOneTwoSat( const Puzzle& problem,
                           Board& correctBoard, 
                           Board& oneSolution, 
                           bool& isFindSolution, 
                           Board& ifGuessBlackBoard, 
                           Board& ifGuessWhiteBoard, 
                           int x, 
                           int y 
                         ) ;

inline int twoSatSolver( const Puzzle& problem, 
                         Board& correctBoard, 
                         SquareToGo& whereCanIgo 
                       ) ; 

inline int twoSatSolver( const Puzzle& problem, 
                         Board& correctBoard,
                         Board& oneSolution, 
                         bool& isFindSolution, 
                         SquareToGo& whereCanIgo 
                       ) ; 

int searchSolverDfs( const Puzzle& problem,
                     Board& solution,
                     int& nodeCount
                   ) ;

int searchOneSolution( const Puzzle& problem,
                       Board& solution,
                       int& nodeCount
                     ) ;

int searchTwoSolutions( bool& isFindSolution, 
                        const Puzzle& problem,
                        Board& solution,
                        Board& oneSolution,
                        int& nodeCount 
                      ) ;
//for TAAI
inline int resumableSolver( const char* fileName, 
                     const Puzzle& problem, 
                     const int nodeCount,
                     struct SearchStack& Stack
                     ) ;

inline int resumableDfs( const char* fileName,
         const Puzzle& problem,
         SearchStack& Stack
         ) ;

void scheduledSolver(const Puzzle problems[NUM_OF_QUESTIONS],int nodeLimit);

#endif

