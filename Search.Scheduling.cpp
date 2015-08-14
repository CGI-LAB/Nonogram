#include "SearchSolver.h"
#include "LineSolver.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <ctime>

inline int resumableSolver(const char* fileName, 
    const Puzzle& problem,
    const int nodeLimit,
    struct SearchStack& stack)
{
    int state;
    if( false == stack.load(fileName) )
    {
      stack.initial();
      state = lineSolver( problem, stack.solution ) ;
      if ( state == SOLVED )
        return SOLVED ;
    }

    stack.nodeLimit = nodeLimit;
    do
    {
        state = resumableDfs(fileName, problem, stack);
    }
    while (state == UNSOLVED);  
    return state;
}

inline int resumableDfs(const char* fileName,
    const Puzzle& problem,
    SearchStack& stack) 
{   
    
    switch(stack.step[ stack.depth])
    {
    case 0:
        stack.state = beginFullyProbing( problem, 
            stack.solution,  
            stack.whereCanIgo[stack.depth], 0, 1,
			stack.solution2
			) ;
        stack.nodeLimit--;    
        stack.nodeCount++;    
        if( SOLVED == stack.state)
            return stack.state;
        else if( CONFLICT == stack.state)
        {
            stack.step[ stack.depth]  = 0;
            stack.depth--;
        } 
        else 
        {
            stack.solution = stack.whereCanIgo[stack.depth].firstGo;
            stack.step[ stack.depth] = 1;
            stack.depth++;      
        }
        if(stack.nodeLimit < 0)
        {
            stack.save(fileName);
            return TIMEOUT;
        }
        break;
    case 1:
        stack.solution= stack.whereCanIgo[stack.depth].secondGo;
        stack.step[ stack.depth] = 2;
        stack.depth++;
        break;
    case 2:
        stack.step[ stack.depth] = 0;
        stack.depth--;
        break;
    default:
        fprintf( stderr, "Bug apperr at depth %d \n", stack.depth) ;
        stack.depth--;
        break;
    }
    if(stack.depth < 0)
        return CONFLICT;
    else
        return UNSOLVED;

}

struct QuestionStruct
{
  bool isSolved ;
  int number ;
  short score ;
 } ;

void InitQuestionStruct( QuestionStruct questions[NUM_OF_QUESTIONS] )
{
  for ( int i = 0 ; i < NUM_OF_QUESTIONS ; i++ )
  {
    questions[i].isSolved = false ;
    questions[i].number = i ;
    questions[i].score = 0 ;
  }
}

int compareQuestion(const void *a, const void *b)
{
  QuestionStruct* pA = (QuestionStruct*) a ;
  QuestionStruct* pB = (QuestionStruct*) b ;
  return pB->score - pA->score ;
}

void scheduledSolver(const Puzzle problems[NUM_OF_QUESTIONS], int nodeLimit )
{
  char filename[10];
  struct SearchStack stack;
  FILE* fp_log = fopen( "logS.txt", "w" ) ;
  QuestionStruct allQuestions[NUM_OF_QUESTIONS];
  InitQuestionStruct( allQuestions ) ;

  clock_t begin = clock() ;

  int count = 0;
  bool first = true ;
  while ( count != NUM_OF_QUESTIONS )
  {
    int index = 0 ;
    for ( int i = 0 ; i < NUM_OF_QUESTIONS ; i++ )
    { 
      clock_t starttime = clock() ;
      if ( !allQuestions[i].isSolved )
      {
        index++ ;
        int questionId = allQuestions[i].number ;
        sprintf( filename, "%d.dat\0", questionId ) ;
        int result = resumableSolver( filename, problems[questionId], nodeLimit, stack );
        if ( result == SOLVED )
        {
          fprintf( stderr, "#%d solved!!!\n", questionId + 1 ) ;
          sprintf( filename, "sol%d.txt", questionId + 1 ) ;
          freopen( filename, "w", stdout ) ;
          printBoardTaai( stack.solution ) ;
          fflush( stdout ) ;
          allQuestions[i].isSolved = true ;
          count++;
        }

        if ( nodeLimit == 0 )
          allQuestions[i].score = stack.solution.numOfPaintedSquares ;

        fprintf( fp_log, "#%d\t%f\t%d\n",questionId + 1, (double)(clock() - starttime)/CLOCKS_PER_SEC, result) ;
      }//end if
      if ( !first && index >= SCHDULE_NUM_QUESTIONS )
        break ;
    }//end for

    fprintf( fp_log, "///////////////////   solved: %d, limited: %d   ////////////////////\n",count, nodeLimit) ;
    fflush( fp_log );

    if ( first )
    {
      qsort( allQuestions, NUM_OF_QUESTIONS, sizeof(QuestionStruct), compareQuestion ) ;
      if ( nodeLimit == LIGHT_NODE_LIMITED )
      {
        first = false ;
        nodeLimit = HEAVT_NODE_LIMITED ;
      }
      else
        nodeLimit = LIGHT_NODE_LIMITED ;
    }

  }//end while

  writeResult() ;
  fprintf( fp_log, "total time usage = %f\n" , (double)(clock() - begin)/CLOCKS_PER_SEC ) ;
  fclose( fp_log );

}
