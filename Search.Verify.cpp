#include "SearchSolver.h"
#include "lineSolver.h"
#include <queue>

static Board ifGuessBlackBoard[P_SIZE][P_SIZE] ;
static Board ifGuessWhiteBoard[P_SIZE][P_SIZE] ;

inline int solveOneTwoSat( const Puzzle& problem,
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
	    	LineMask oneRow = ifGuessBlackBoard.rowString[i] | ifGuessWhiteBoard.rowString[i] ;
		    if ( oneRow != correctBoard.rowString[i] )
		    {
          isUpdate = true ;
          correctBoard.numOfSquareOnBoard += count64( ( correctBoard.rowString[i] ^ oneRow ) ) ;
          correctBoard.rowString[i] = correctBoard.rowString[i] & oneRow ;
		      NodeQueue::myQ.pushQ(i + P_SIZE) ;
		    }

		    LineMask oneCol = ifGuessBlackBoard.colString[i] | ifGuessWhiteBoard.colString[i] ;
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
        if ( memcmp( oneSolution.rowString, resumeBoard.rowString,  P_SIZE * 8  ) != 0 )
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

inline int twoSatSolver( const Puzzle& problem, 
                         Board& correctBoard,
                         Board& oneSolution, 
                         bool& isFindSolution, 
                         SquareToGo& whereCanIgo 
                       )
{
  Board resumeBoard ;

  if ( correctBoard.numOfSquareOnBoard == S_SIZE )
  {
    if ( !isFindSolution)
    {
      isFindSolution = true ;
      oneSolution = correctBoard ;
      return SOLVED ;
    } // end if
    else
    {
      if ( memcmp( oneSolution.rowString, correctBoard.rowString,  P_SIZE * 8  ) != 0 )
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
          int state = solveOneTwoSat( problem,
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
              if ( memcmp( oneSolution.rowString, correctBoard.rowString,  P_SIZE * 8  ) != 0 )
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

    if ( memcmp( resumeBoard.rowString, correctBoard.rowString, P_SIZE * 8 ) == 0 ) 
    {
      int x = 0 ;
      int y = 0 ;
      double maxScore = -1 ;
      for ( int i = 0 ; i < P_SIZE ; i++ )
      {
        for ( int j = 0 ; j < P_SIZE ; j++ )
        {
          if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN ) 
          {
            double score = min( ifGuessBlackBoard[i][j].numOfSquareOnBoard, ifGuessWhiteBoard[i][j].numOfSquareOnBoard ) + 
                           1.85 * log ( 1.0 + (double)abs( ifGuessBlackBoard[i][j].numOfSquareOnBoard - ifGuessWhiteBoard[i][j].numOfSquareOnBoard ) );

            if ( score > maxScore )
            {
              x = i ;
              y = j ;
              maxScore = score ;
            }
          }
        }
      }

      //if ( ifGuessWhiteBoard[x][y].numOfSquareOnBoard > ifGuessBlackBoard[x][y].numOfSquareOnBoard )
      //{
        whereCanIgo.firstGo = ifGuessWhiteBoard[x][y];
        whereCanIgo.secondGo = ifGuessBlackBoard[x][y];
      //}
      //else
     // {      
      //  whereCanIgo.firstGo = ifGuessBlackBoard[x][y];
      //  whereCanIgo.secondGo = ifGuessWhiteBoard[x][y];
      //}
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
  int state = twoSatSolver( problem, solution, oneSolution, isFindSolution, whereCanIgo ) ;
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
int searchTwoSolutions( bool& isFindSolution, 
                        const Puzzle& problem,
                        Board& solution,
                        Board& oneSolution,
                        int& nodeCount 
                      ) 
{
  int state = lineSolver( problem, solution ) ;
  if ( state == SOLVED )
    return SOLVED ;        
  else if ( state == CONFLICT)
    return CONFLICT ;
  else
    return VerifiedManySolution( isFindSolution, problem, solution, oneSolution, nodeCount ) ;
}