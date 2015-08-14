#include <cstdio>
#include <iostream>
#include "bit.h"
#include "puzzle.h"
#include "LineSolver.h"
#include "lineMask.h"
#include "config.h"
#include "Parsers.h"
#include "SearchSolver.h"
#include <ctime>
// #include "Dt.h"
#include "Hash.h"

#if defined(_MSC_VER)
#include <Windows.h>
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 
#endif


char inputname[] = INPUT_FILE ;
char outputname[] = OUTPUT_FILE ;
char logname[] = LOG_FILE ;
int currentId = 0 ;
#if MODE == 0 
BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType)
{
  writeResult() ;
  exit(0) ;
  return TRUE;
}

void initialFile()
{
  system( "DEL *.dat" ) ;
  system( "DEL sol*.txt" ) ;
}
#endif

int gLimitedLevel = -1 ;
int gNumOfWhiteConflict = 0 ;
int gNumOfBlackConflict = 0 ;
int gNumOfMerge = 0 ;
int gNumOfNoop = 0 ;
int gDiff = 9999999 ;

int myrand()
{
    static int seed = 0; 
    // rand formula from FreeBSD rand.c
    seed = seed * 1103515245 + 12345;
    return seed % ((unsigned long)RAND_MAX + 1);
}


int main(int argc, char *argv[]){
#ifdef RANDOM_MOVE 
    srand(myrand());
#endif
	initialLineMask();
	initialHash();
	
#if defined(_MSC_VER)
    system( "MKDIR PRE_SEARCH_DETAIL" ) ;
    system( "MKDIR BRFORE_PRE_SEARCH" ) ;
    system( "MKDIR SOLUTION" ) ; 
    system( "MKDIR DT" ) ; 
    system( "DEL /Q TWO_SAT\\*" ) ;
    system( "DEL /Q PRE_SEARCH_DETAIL\\*" ) ;
    system( "DEL /Q BRFORE_PRE_SEARCH\\*" ) ;
    system( "DEL /Q SOLUTION\\*" ) ; 
    system( "DEL /Q DT\\*" ) ; 
#else
    system( "mkdir -p PRE_SEARCH_DETAIL");
    system( "mkdir -p BRFORE_PRE_SEARCH" ) ;
    system( "mkdir -p SOLUTION" ) ; 
    system( "mkdir -p cd DT" ) ; 
    system( "rm TWO_SAT/*" ) ;
    system( "rm PRE_SEARCH_DETAIL/*" ) ;
    system( "rm BRFORE_PRE_SEARCH/*" ) ;
    system( "rm SOLUTION/*" ) ; 
    system( "rm DT/*" ) ;
#endif


#if defined(_MSC_VER)
   // SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  #if MODE == 0 
    SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
    initialFile() ;
  #endif
#endif
  if ( argc == 2 )
    gLimitedLevel = atoi(argv[1]) ;
	if( argc > 2)
	{ /*there is arg*/
		Puzzle puzzle( argv[2] );
		Board board1;
		initBoard( board1 ) ;
		Board board2;
		initBoard( board2 ) ;

		if ( strcmp( argv[1], "SOLVE" ) == 0 )
		{
			int nodeCount = 0 ;
			clock_t starttime = clock();
		  int state = searchOneSolution( puzzle, board1, board2, nodeCount ) ;
			printf("%d\t%.3f\n", nodeCount, (double)(clock() - starttime)/CLOCKS_PER_SEC) ; 
			printBoardTaai( board1 ) ;

			fflush( stdout) ;
		} // end if is solve
		/* else if ( strcmp( argv[1], "GENERATE" ) == 0 )
		{
			int nodeCount = 0 ;
			bool isFind = false ;
			Board oneBoard ;
			initBoard( oneBoard ) ;
			int state = searchTwoSolutions( isFind, puzzle, board1,  oneBoard, nodeCount ) ;
            if ( state == SOLVED )
            {
                    // printf("%d\n", nodeCount ) ; 
                Board b1, b2 ;
                initBoard( b1 ) ;
                initBoard( b2 ) ;
                int nodeCount = 0 ;
                searchOneSolution( puzzle, b1, b2, nodeCount ) ;
            // searchBy2Sat( puzzle, b2 ) ;
                printf( "%d\n", b1.numOfPaintedSquares - b2.numOfPaintedSquares ) ;

            }
			else if ( state == CONFLICT )
				printf( "-1\n" ) ;
			else if ( state == MANY_SOLUTION )
				printf( "-2\n") ;
			fflush( stdout ) ;
		} */ // end else if is generate puzzle
	} 
	else
	{
    Puzzle quests[NUM_OF_QUESTIONS];
    parseTaai(inputname,quests);
#if MODE == 0 
    scheduledSolver(quests, 0) ;
#else
    Board b1[NUM_OF_QUESTIONS];
	Board b2[NUM_OF_QUESTIONS];
    freopen( OUTPUT_FILE, "w", stdout ) ;
    FILE* log = fopen( logname, "w" ) ;
		for(int i = 0; i < NUM_OF_QUESTIONS ; i++)
    {
      int nodeCount = 0 ;
      clock_t starttime = clock() ;

      gNumOfWhiteConflict = 0 ;
      gNumOfBlackConflict = 0 ;
      gNumOfMerge = 0 ;
      gNumOfNoop = 0 ;
      gDiff = 99999999 ;

 #if MODE==1 || MODE==2 || MODE==3
      initBoard( b1[i] ) ;
	  initBoard( b2[i] ) ;
	  b1[i].numOfPaintedSquares = 0;
	  b2[i].numOfPaintedSquares = 0;
      //printf("=====================$%d=====================\n",i+1);
      int state = searchOneSolution( quests[i], b1[i], b2[i],nodeCount ) ;
      
      // Board oneBoard ;
      // initBoard(oneBoard);
      // bool isFind = false ;
      //int state = searchTwoSolutions( isFind, quests[i], b[i], oneBoard, nodeCount ) ;
      
	  if (b2[i].numOfPaintedSquares == S_SIZE && b1[i].numOfPaintedSquares == S_SIZE)
	  {
		  fprintf( stderr, "#%d solved, multi!!!\n", i + 1 ) ;
		  fprintf(stdout, "$%d multi\n", i + 1) ;
		  printPuzzle( quests[i], b1[i] );
		  printf("\n");
		  printPuzzle( quests[i], b2[i] );
	  } else if (b1[i].numOfPaintedSquares == S_SIZE) {
		  fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;
		  fprintf(stdout, "$%d solved\n", i + 1) ;
		  // printBoardTaai( b[i] ) ;
		  printPuzzle( quests[i], b1[i] );
	  } else if (b2[i].numOfPaintedSquares == S_SIZE) {
		  fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;
		  fprintf(stdout, "$%d solved\n", i + 1) ;
		  // printBoardTaai( b[i] ) ;
		  printPuzzle( quests[i], b2[i] );
		  b1[i] = b2[i];
	  } else {
		  fprintf( stderr, "#%d unsolved!!!\n", i + 1 ) ;
		  fprintf(stdout, "$%d unsolved\n", i + 1) ;
		  printPuzzle( quests[i], b1[i] );
	  }
      
      fprintf(log, "#%d\t%d\t%d\t%d\t%d\t%f\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%.2f\n", i + 1, b1[i].numOfPaintedSquares, countPaintedCells(b1[i]), state, nodeCount, (double)(clock() - starttime)/CLOCKS_PER_SEC, NodeQueue::myQ.totalHit, (NodeQueue::myQ.totalQuery-NodeQueue::myQ.totalHit), NodeQueue::myQ.totalProbing, NodeQueue::myQ.totalMerging , NodeQueue::myQ.totalPaintedByLineSolver, 
				NodeQueue::myQ.layerNodeCount, NodeQueue::myQ.layerAverage
		) ;
      
		/*
		  printBoardTaai( oneBoard ) ;
      if (state == MANY_SOLUTION) 
        printBoardTaai( b[i] ) ;
      */

	  // fprintf(log, "#%d\t%d\t%d\t%d\t%f\n", i + 1, b[i].numOfPaintedSquares, state, nodeCount, (double)(clock() - starttime)/CLOCKS_PER_SEC ) ;
      
      //fprintf(log, "$%d\t%d\t%d\t%d\t%d\n", i + 1, gNumOfWhiteConflict, gNumOfBlackConflict, gNumOfMerge, gNumOfNoop ) ;
      //printBoardTaai(b[i]) ;
      
      //UglyDt( quests[i], b[i] ) ;
      /*

      
      char tmp[100] ;
      sprintf( tmp, "SOLUTION/$%d.txt", i + 1 ) ;
      freopen( tmp, "a", stdout ) ;
      Board oneBoard ;
      bool isFind = false ;
      initBoard( b[i] ) ;
      initBoard( oneBoard ) ;
      nodeCount = 0 ;
      state = searchTwoSolutions( isFind, quests[i], b[i], oneBoard, nodeCount ) ;
      fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;

		  printPuzzle( quests[i], oneBoard ) ;
      if (state == MANY_SOLUTION) 
        printPuzzle( quests[i], b[i] ) ;
      else if ( state == CONFLICT )
        printf( "Conflict!\n" ) ;
      //printf("$%d\n",i+1);
		 // printBoardTaai(b[i] ) ;
      //printPuzzle( quests[i], b[i] ) ;
      //printf("=============================================\n\n\n",i+1);
      */
 /* #elif MODE==2
      Board oneBoard ;
			initBoard( oneBoard ) ;
      initBoard( b[i] ) ;
      bool isFind = false ;
      int state = searchTwoSolutions( isFind, quests[i], b[i], oneBoard, nodeCount ) ;
      fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;
      printf("$%d\n", i+1);
	  // printPuzzle( quests[i], oneBoard ) ;
      printBoardTaai(oneBoard);
      if (state == MANY_SOLUTION) printPuzzle( quests[i], b[i] ) ;
      //  printBoardTaai(b[i]);
      fprintf( log, "#%d\t clock:%f\t state:%d\t node:%d\n",i + 1 , (double)(clock() - starttime)/CLOCKS_PER_SEC, state, nodeCount ) ;
	  fflush(log);*/
      
 #endif
      //fprintf( log, "#%d\t%f\n",i + 1 , (double)(clock() - starttime)/CLOCKS_PER_SEC ) ;
	  fflush( stdout ) ;
      fflush( log ) ;
    }
#endif
  }
	
  return 0;
}

