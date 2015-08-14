#ifndef _SEARCHSOLVER_H_
#define _SEARCHSOLVER_H_

#include "puzzle.h"
#include <algorithm>
#include <queue>
#include <list>
#include "Dependency.h"
// #include "Dt.h"
using namespace std ;

#define _CRT_SECURE_NO_WARNINGS


struct InverseNode
{
  Board* board ;
  int squareType ;
  int x ;
  int y ;
  int startIndex ;
} ;


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
	Board solution2; 
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

inline int beginOneProbe( const Puzzle& problem,
                           Board& correctBoard, 
                           PresearchBoard& ifGuessBlackBoard, 
                           PresearchBoard& ifGuessWhiteBoard, 
                           int x, 
                           int y 
                         ) ;

inline int beginOneProbe( const Puzzle& problem,
                           Board& correctBoard, 
                           Board& oneSolution, 
                           bool& isFindSolution, 
                           Board& ifGuessBlackBoard, 
                           Board& ifGuessWhiteBoard, 
                           int x, 
						   int y // , 
						   // bool qFirstRun
                         ) ;

inline int beginFullyProbing( const Puzzle& problem, 
                         Board& correctBoard, 
                         SquareToGo& whereCanIgo, 
                         int nodeCount, 
						 int pLevel,
						 Board& solution2
                       ) ; 

// for verify..
inline int beginFullyProbing( const Puzzle& problem, 
                         Board& correctBoard,
                         Board& oneSolution, 
                         bool& isFindSolution, 
                         SquareToGo& whereCanIgo 
                       ) ; 

int searchSolverDfs_Wrapper( const Puzzle& problem,
	Board& solution1,
	Board& solution2,
	int& nodeCount,
	int level 
	) ;

int searchSolverDfs_Main( const Puzzle& problem,
                     Board& solution1,
					 Board& solution2,
                     int& nodeCount,
                     int level 
                   ) ;

int searchOneSolution( const Puzzle& problem,
                       Board& solution1,
					   Board& solution2,
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

bool checkDt(const Puzzle problem, Board& solution) ;

void selectMove( const Puzzle &problem, Board &correctBoard, Board &origBoard, int& x, int& y) ;

int searchBy2SatOrDT( const Puzzle& problem, Board& solution ) ;

inline int updateInverseList(const Puzzle &problem, UpdateList& updateList, Board& setBoard, int pX, int pY ) ;
inline void initialPresearch( Board &correctBoard ) ;

inline bool checkWhetherInList(PresearchBoard &pUpdateList, int pX, int pY, int pType);

inline bool checkWhetherInList2(UpdateList& updateList, int pX, int pY, int pType);

inline bool checkWhetherBoardsAreSame(Board &b1, Board &b2);

inline bool mergeBoard( Board &dstBoard, Board &srcBoard);
inline bool mergeBoard_Detail( Board &dstBoard, Board &srcBoard, bool putQueue, bool getList, list<OneCell> &pList);
inline bool mergeBoard( Board &dstBoard, ShortBoard &srcBoard, bool putQueue);

inline void sendInverseLinksWhite( PresearchBoard &ifGuessWhiteBoard, int pX, int pY );

inline void sendInverseLinksBlack( PresearchBoard &ifGuessBlackBoard, int pX, int pY );

#ifdef ENABLE_BACKJUMPING
void updateJumpLevelTable(Board &targetBoard, int level, bool clean);
void updateJumpLevelTableWithAnd(Board &targetBoard, int level);
void cleanJumpLevelTable(Board &targetBoard, int level);
#endif

void pushProbingNodes(int pLevel, Board &pBoard, Board &pFirstBoard);
void popProbingNodes(int pLevel, Board &pBoard);
#endif

