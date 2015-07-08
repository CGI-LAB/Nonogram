#include "SearchSolver.h"
#include "lineSolver.h"
#include <queue>
#include <cstdio>


static Board ifGuessBlackBoard[P_SIZE][P_SIZE] ;
static Board ifGuessWhiteBoard[P_SIZE][P_SIZE] ;
DependencyTable dbTable;

inline int solveOneTwoSat( const Puzzle& problem,
    Board& correctBoard, 
    Board& ifGuessBlackBoard, 
    Board& ifGuessWhiteBoard, 
    int x, 
    int y 
    ) 
{
    Board resumeBoard = correctBoard ;
    setAndFlag( x, y, correctBoard, SQUARE_SPACE ) ;
    
    dbTable.thisTempTable = 0;
    int state = lineSolver( problem, correctBoard ) ;
    LineMask spaceTable = dbTable.thisTempTable;
    dbTable.thisUpdateTable |= spaceTable;
    if ( state == SOLVED )
        return SOLVED ;
    else if ( state == CONFLICT )
    {
        
        correctBoard = resumeBoard ;
        setAndFlag( x, y, correctBoard, SQUARE_BLOCK ) ;
        dbTable.thisTempTable = 0;
        int state = lineSolver( problem, correctBoard ) ;
        dbTable.tempUpdateTable |= dbTable.thisTempTable;

        if ( state == SOLVED )
            return SOLVED ;
        else if ( state == CONFLICT )
        {
            return CONFLICT ;
        } // else if is conflict
    } // else if guess white is conflict
    else
    {
        ifGuessWhiteBoard = correctBoard ;
        correctBoard = resumeBoard ;
        setAndFlag( x, y, correctBoard, SQUARE_BLOCK ) ;
        dbTable.thisTempTable = 0;
        int state = lineSolver( problem, correctBoard ) ;
        dbTable.thisUpdateTable |=  dbTable.thisTempTable;
        if ( state == SOLVED )
            return SOLVED ;
        else if ( state == CONFLICT )
        {
            correctBoard = ifGuessWhiteBoard ;
            dbTable.tempUpdateTable |= spaceTable;
        } // else if is conflict
        else
        {
            ifGuessBlackBoard = correctBoard ;
            correctBoard = resumeBoard ;

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
                dbTable.thisTempTable = 0;
                int state = lineSolver( problem, correctBoard ) ;
                dbTable.tempUpdateTable |= dbTable.thisTempTable;
                if ( state == SOLVED )
                    return SOLVED ;
                else if ( state == CONFLICT )
                    return CONFLICT ;                
            }

        } // else do merege
    } // else is guess black 


    return UNSOLVED ;
}

inline int twoSatSolver( const Puzzle& problem, 
    Board& correctBoard, 
    SquareToGo& whereCanIgo 
    )
{
    Board resumeBoard ;
    dbTable.updateTable = 0 ;   
    while ( true )
    {
        dbTable.tempUpdateTable = 0;
        resumeBoard = correctBoard ;
        for ( int i = 0 ; i < P_SIZE ; i++ )
        {
            for ( int j = 0 ; j < P_SIZE ; j++ )
            {
                if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN &&( ( dbTable.updateTable & dbTable.pointTables[i][j]) || ( dbTable.updateTable == 0 ))
                        )  
                {                                                      
                        dbTable.thisUpdateTable = 0;
                        int state = solveOneTwoSat( problem,
                            correctBoard, 
                            ifGuessBlackBoard[i][j],
                            ifGuessWhiteBoard[i][j],
                            i,
                            j
                            ) ;
                        if ( state == SOLVED )
                            return SOLVED ;
                        else if ( state == CONFLICT )
                            return CONFLICT ;
                        dbTable.pointTables[i][j] = dbTable.thisUpdateTable;
                }
            } // for j ~ puzzle size
        } // for i ~ puzzle size       
        dbTable.updateTable |= dbTable.tempUpdateTable;
        if ( dbTable.tempUpdateTable == 0 ) 
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

            //dbTable.updateTable |= dbTable.pointTables[x][y];
            whereCanIgo.firstGo = ifGuessWhiteBoard[x][y];
            whereCanIgo.secondGo = ifGuessBlackBoard[x][y];

            return UNSOLVED ;
        } // end if 2sat is stall
        
    } // end while


    return UNSOLVED ;
}




int searchOneSolution( const Puzzle& problem,
    Board& solution,
    int& nodeCount
    ) 
{

    memset(dbTable.pointTables,0,sizeof(dbTable.pointTables));
   // dbTable.updateTable = 0 ;
    int state = lineSolver( problem, solution ) ;
    if ( state == SOLVED )
        return SOLVED ;
    else if ( state == CONFLICT)
        return CONFLICT ;
    else
        return searchSolverDfs( problem, solution, nodeCount ) ;
}



int searchSolverDfs( const Puzzle& problem,
    Board& solution, 
    int& nodeCount
    )
{
    SquareToGo whereCanIgo ;

    nodeCount++ ;
    int state = twoSatSolver( problem, solution, whereCanIgo ) ;
    if ( state == SOLVED )
        return SOLVED ;
    else if ( state == CONFLICT )
        return CONFLICT ;


    solution = whereCanIgo.firstGo  ;
    state = searchSolverDfs( problem, solution, nodeCount ) ;
    if ( state == SOLVED )
    {
        return SOLVED ;
    }
    else
    {
        solution = whereCanIgo.secondGo ;
        //NodeQueue::myQ.initial() ;
        return searchSolverDfs( problem, solution, nodeCount ) ;
    } // end else is not solved
}

void writeResult() 
{
  FILE* output = fopen( OUTPUT_FILE, "w" ) ;
  for ( int i = 0 ; i < NUM_OF_QUESTIONS ; i++ )
  {
    char tmp[10] ;
    sprintf( tmp, "sol%d.txt", i + 1 ) ;
    FILE* input = fopen( tmp, "r" ) ;
    fprintf(output, "$%d\n", i + 1) ;
    fflush(output) ;
    if ( input != NULL )
    {
      char buffer[4096] ;
      memset( buffer, 0, sizeof( buffer ) ) ;
      size_t n = fread(buffer, 1, sizeof(buffer), input );
      fwrite(buffer, 1, n, output );
      fclose( input ) ;
    }
    else
    {
      for ( int j = 0 ; j < P_SIZE ; j++ )
      {
        for ( int k = 0 ; k < P_SIZE ; k++ )
        {
          if ( k < P_SIZE - 1 )
            fprintf( output, "0\t" ) ;
          else
            fprintf( output, "0\n" ) ;
          fflush(output) ;
        }
      }
    }
  }

  fclose( output ) ;
}