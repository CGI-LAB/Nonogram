#include "SearchSolver.h"
#include "LineSolver.h"
#include <queue>

static Board ifGuessBlackBoard[P_SIZE][P_SIZE] ;
static Board ifGuessWhiteBoard[P_SIZE][P_SIZE] ;

inline int beginOneProbe( const Puzzle& problem,
                           Board& correctBoard, 
                           Board& oneSolution, 
                           bool& isFindSolution, 
                           Board& ifGuessBlackBoard, 
                           Board& ifGuessWhiteBoard,
                           int x, 
                           int y 
                         ) 
{
  Board resumeBoard = correctBoard ;
  setAndFlag( x, y, correctBoard, SQUARE_SPACE ) ;
  int state1 = lineSolver( problem, correctBoard ) ;
  ifGuessWhiteBoard = correctBoard ;

  correctBoard = resumeBoard ;
  setAndFlag( x, y, correctBoard, SQUARE_BLOCK ) ;
  int state2 = lineSolver( problem, correctBoard ) ;
  ifGuessBlackBoard = correctBoard ;

  correctBoard = resumeBoard ;

  if ( state1 == SOLVED && state2 == SOLVED )
  {
    oneSolution = ifGuessBlackBoard;
    correctBoard = ifGuessWhiteBoard; 
    return MANY_SOLUTION ;
  }  
  else if ( state1 != SOLVED && state2 != SOLVED )
  {
    if ( state1 == CONFLICT && state2 == CONFLICT )
      return CONFLICT ;
    else if ( state1 == CONFLICT )
      correctBoard = ifGuessBlackBoard ;
    else if ( state2 == CONFLICT )
      correctBoard = ifGuessWhiteBoard ;
    else if ( state1 == UNSOLVED && state2 == UNSOLVED)
    {
      bool isUpdate = false ;
      for ( int i = 0 ; i < P_SIZE ; i++ )
      {
#ifdef SSE_BOARD
	    	LineMask_128 oneRow = ifGuessBlackBoard.rowString[i] | ifGuessWhiteBoard.rowString[i] ;
#else
        LineMask oneRow = ifGuessBlackBoard.rowString[i] | ifGuessWhiteBoard.rowString[i] ;
#endif
		    if ( oneRow != correctBoard.rowString[i] )
		    {
          isUpdate = true ;
#ifdef SSE_BOARD
          correctBoard.numOfPaintedSquares += count128( ( correctBoard.rowString[i] ^ oneRow ) ) ;
#else
          correctBoard.numOfPaintedSquares += count64( ( correctBoard.rowString[i] ^ oneRow ) ) ;
#endif
          correctBoard.rowString[i] = correctBoard.rowString[i] & oneRow ;
		      NodeQueue::myQ.pushQ(i + P_SIZE) ;
		    }
#ifdef SSE_BOARD
		    LineMask_128 oneCol = ifGuessBlackBoard.colString[i] | ifGuessWhiteBoard.colString[i] ;
#else
        LineMask oneCol = ifGuessBlackBoard.colString[i] | ifGuessWhiteBoard.colString[i] ;
#endif
        if ( oneCol != correctBoard.colString[i] )
		    {
          isUpdate = true ;
		      correctBoard.colString[i] = correctBoard.colString[i] & oneCol;
		      NodeQueue::myQ.pushQ(i);
	    	}


      } // for each line

      if ( isUpdate )
      {
        int state = lineSolver( problem, correctBoard ) ;
        if ( state == SOLVED )
          return SOLVED ;
        else if ( state == CONFLICT )
          return CONFLICT ;
      }
    } // else if merge
  }
  else if ( state1 == SOLVED || state2 == SOLVED )
  {
    if ( state1 == CONFLICT )
    {
      correctBoard = ifGuessBlackBoard ;
      return SOLVED ;
    }
    else if ( state2 == CONFLICT )
    {
      correctBoard = ifGuessWhiteBoard ;
      return SOLVED ;
    }
    else
    {
      if ( state1 == SOLVED )
        resumeBoard = ifGuessWhiteBoard ;
      else if ( state2 == SOLVED )
        resumeBoard = ifGuessBlackBoard;
      if ( !isFindSolution)
      {
        isFindSolution = true ;
        oneSolution = resumeBoard ;
        return UNSOLVED ;
      } // end if
      else
      {
        if ( memcmp( oneSolution.rowString, resumeBoard.rowString,  sizeof(resumeBoard.rowString)  ) != 0 )
        {
          correctBoard = resumeBoard ;
          return MANY_SOLUTION ;
        }
        else
          return UNSOLVED ;
      } // end else
    }
  }

  return UNSOLVED ;
}

inline int beginFullyProbing( const Puzzle& problem, 
                         Board& correctBoard,
                         Board& oneSolution, 
                         bool& isFindSolution, 
                         SquareToGo& whereCanIgo 
                       )
{
  Board resumeBoard ;
  Board origBoard = correctBoard;

  if ( correctBoard.numOfPaintedSquares == S_SIZE )
  {
    if ( !isFindSolution)
    {
      isFindSolution = true ;
      oneSolution = correctBoard ;
      return SOLVED ;
    } // end if
    else
    {
      if ( memcmp( oneSolution.rowString, correctBoard.rowString,  sizeof(correctBoard.rowString) ) != 0 )
        return MANY_SOLUTION ;
      else
        return SOLVED ;
    } // end else
  }

  while ( true )
  {
    resumeBoard = correctBoard ;
    for ( int i = 0 ; i < P_SIZE ; i++ )
    {
      for ( int j = 0 ; j < P_SIZE ; j++ )
      {
        if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN ) 
        {
          int state = beginOneProbe( problem,
                                      correctBoard, 
                                      oneSolution,
                                      isFindSolution,
                                      ifGuessBlackBoard[i][j],
                                      ifGuessWhiteBoard[i][j],
                                      i,
                                      j
                                    ) ;
          if ( state == SOLVED )
          {
            if ( !isFindSolution)
            {
              isFindSolution = true ;
              oneSolution = correctBoard ;
              return SOLVED ;
            } // end if
            else
            {
              if ( memcmp( oneSolution.rowString, correctBoard.rowString,  sizeof(correctBoard.rowString)  ) != 0 )
                return MANY_SOLUTION ;
              else
                return SOLVED ;
            } // end else
          }
          else if ( state == CONFLICT )
            return CONFLICT ;
          else if ( state == MANY_SOLUTION )
          {
            return MANY_SOLUTION ;
          }
        } // end if is unknown
      } // for j ~ puzzle size
    } // for i ~ puzzle size


    if ( memcmp( resumeBoard.rowString, correctBoard.rowString, sizeof(correctBoard.rowString) ) == 0 ) 
    {
      int x = -1 ;
      int y = -1 ;
      selectMove(problem, correctBoard, origBoard, x, y) ;
	  if (x!=-1 && y!=-1)
	  {
		whereCanIgo.secondGo = ifGuessWhiteBoard[x][y];
		whereCanIgo.firstGo = ifGuessBlackBoard[x][y];
	  } else {
		whereCanIgo.firstGo.numOfPaintedSquares = -1;
		whereCanIgo.secondGo.numOfPaintedSquares = -1;
	  }
      return UNSOLVED ;
    } // end if 2sat is stall
   
  } // end while


  return UNSOLVED ;
}

int VerifiedManySolution( bool& isFindSolution, 
                          const Puzzle& problem,
                          Board& solution, 
                          Board& oneSolution, 
                          int& nodeCount 
                        ) 
{
  nodeCount++ ;
  SquareToGo whereCanIgo ;
  int state = beginFullyProbing( problem, solution, oneSolution, isFindSolution, whereCanIgo ) ;
  if ( state == SOLVED )
    return SOLVED ;
  else if ( state == CONFLICT )
    return CONFLICT ;
  else if ( state == MANY_SOLUTION )
    return MANY_SOLUTION ;

  solution = whereCanIgo.firstGo ;
  int state1 = VerifiedManySolution( isFindSolution, 
                                     problem,
                                     solution, 
                                     oneSolution, 
                                     nodeCount 
                                   ) ;
 // NodeQueue::myQ.initial() ;
  if ( state1 == MANY_SOLUTION )
    return MANY_SOLUTION ;


  solution = whereCanIgo.secondGo ;
  int state2 = VerifiedManySolution( isFindSolution, 
                                     problem,
                                     solution, 
                                     oneSolution, 
                                     nodeCount 
                                   ) ;
 // NodeQueue::myQ.initial() ;
  if ( state2 == MANY_SOLUTION )
    return MANY_SOLUTION ;

  if ( state1 == CONFLICT && state2 == CONFLICT) 
    return CONFLICT;
  else if (state1 == SOLVED && state2 == SOLVED )
    return MANY_SOLUTION;
  else
    return SOLVED ;
}

extern int currentId ;
int searchTwoSolutions( bool& isFindSolution, 
                        const Puzzle& problem,
                        Board& solution,
                        Board& oneSolution,
                        int& nodeCount 
                      ) 
{
  currentId++ ;
  int state = lineSolver( problem, oneSolution ) ;
  if ( state == SOLVED )
    return SOLVED ;        
  else if ( state == CONFLICT)
    return CONFLICT ;
  else
    return VerifiedManySolution( isFindSolution, problem, solution, oneSolution, nodeCount ) ;
}
