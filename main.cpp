#include <cstdio>
#include <iostream>
#include "bit.h"
#include "puzzle.h"
#include "lineSolver.h"
#include "config.h"
#include "Parsers.h"
#include "SearchSolver.h"
#include <ctime>

#include "hash.h"
#if defined(_MSC_VER)
#include <Windows.h>
#endif


char inputname[] = INPUT_FILE ;
char outputname[] = OUTPUT_FILE ;
char logname[] = LOF_FILE ;
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

int main(int argc, char *argv[]){

	initialHash();
#if defined(_MSC_VER)
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  #if MODE == 0 
    SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
    initialFile() ;
  #endif
#endif
	if( argc > 2)
	{ /*there is arg*/
		Puzzle puzzle( argv[2] );
		Board board;
		initBoard( board ) ;

		if ( strcmp( argv[1], "SOLVE" ) == 0 )
		{
			int nodeCount = 0 ;
			clock_t starttime = clock();
		  int state = searchOneSolution( puzzle, board, nodeCount ) ;
			printf("%d\t%.3f\n", nodeCount, (double)(clock() - starttime)/CLK_TCK) ; 
			printBoardTaai( board ) ;

			fflush( stdout) ;
		} // end if is solve
		else if ( strcmp( argv[1], "GENERATE" ) == 0 )
		{
			int nodeCount = 0 ;
			bool isFind = false ;
			Board oneBoard ;
			initBoard( oneBoard ) ;
			int state = searchTwoSolutions( isFind, puzzle, board, oneBoard, nodeCount ) ;
			if ( state == SOLVED )
				printf("%d\n", nodeCount ) ; 
			else if ( state == CONFLICT )
				printf( "-1\n" ) ;
			else if ( state == MANY_SOLUTION )
				printf( "-2\n") ;
			fflush( stdout ) ;
		} // end else if is generate puzzle
	} 
	else
	{
    Puzzle quests[NUM_OF_QUESTIONS];
    parseTaai(inputname,quests);
#if MODE == 0 
    scheduledSolver(quests, 0) ;
#else
    Board b[NUM_OF_QUESTIONS];
    freopen( outputname, "w", stdout ) ;
    FILE* log = fopen( logname, "w" ) ;
    clock_t begin_time = clock();
		for(int i = 0; i < NUM_OF_QUESTIONS ; i++)
    {
      int nodeCount = 0 ;
      clock_t starttime = clock() ;
  #if MODE == 1
      initBoard( b[i] ) ;

      int state = searchOneSolution( quests[i], b[i], nodeCount ) ;
      fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;
      printf("$%d\n",i+1);
		  printBoardTaai(b[i] ) ;
  #elif MODE == 2
      Board oneBoard ;
			initBoard( oneBoard ) ;
      initBoard( b[i] ) ;
      bool isFind = false ;
			
      int state = searchTwoSolutions( isFind, quests[i], b[i], oneBoard, nodeCount ) ;
      fprintf( stderr, "#%d solved!!!\n", i + 1 ) ;
      printf("$%d\n", i+1);
		  printBoardTaai(oneBoard);
      if (state == MANY_SOLUTION)
        printBoardTaai(b[i]);
  #endif
      fprintf( log, "#%d\t%f\n",i + 1 , (double)(clock() - starttime)/CLOCKS_PER_SEC ) ;
	  fflush( stdout ) ;
      fflush( log ) ;
    }
    clock_t end_time = clock();
    fprintf(log, "total time: %f\n", (double)(end_time - begin_time) / CLOCKS_PER_SEC);
    fflush(log) ;
    fclose(log);
#endif
  }
	
  return 0;
}