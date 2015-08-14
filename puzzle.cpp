#include "puzzle.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>

extern LineMask zHashKeyTable[33][65];

extern  struct MyQueue myQ;


Puzzle::Puzzle()
{

}

Puzzle::~Puzzle(){}
Puzzle::Puzzle( const char *sInput)
{
#ifdef _DEBUG
	for(unsigned i = 0 ; i < P_SIZE * 2 ;i++){
		mLines[i].mCount = 0;
	}
#endif
	readBuffer(*this, sInput);

}

int countPaintedCells(Board& sol)
{
	int qCount = 0;
	for(unsigned i = 0; i < P_SIZE; i++){
		for(unsigned j = 0;j < P_SIZE; j++){
			switch ( getSquare(j, i, sol) )
			{
				case SQUARE_UNKNOWN:
					break;

				default:
					++qCount;
					break;
			}
		}
	}
	return qCount;
}

void printPuzzle(const Puzzle& prob,Board& sol){
	for(unsigned i = 0; i < P_SIZE; i++){
		for(unsigned j = 0;j < P_SIZE; j++)
		{
			// if (j==7 && i==12) printf(">");
			switch (getSquare(j, i, sol) )
			{
			case SQUARE_BLOCK: 
				printf("¡½");
				break;
			case SQUARE_SPACE: 
				printf("¢®");
				break;
			case SQUARE_UNKNOWN:
				printf("¡¼");
				break;
			default:
				printf("¦ä");
				break;
			}
			
		}
		for(unsigned j = 0; j <= prob.mLines[ i + P_SIZE ].mCount; j++){
			if(prob.mLines[ i + P_SIZE ].mNumbers[ j ] != 0)
	   		printf("%2u ", prob.mLines[ i + P_SIZE ].mNumbers[ j ]);
		}
		printf("\n",i);
	}

	int maxClue = 0;
	for(unsigned j=0; j<P_SIZE; ++j)
	{
		if (prob.mLines[j].mCount>maxClue) maxClue=prob.mLines[j].mCount;
	}

	for(int i=0; i<=maxClue; ++i)
	{
		for(unsigned j=0; j<P_SIZE; ++j)
		{
			if(i >= prob.mLines[j].mCount){
				printf("  ");
				continue;
			}
			printf("%2d",prob.mLines[j].mNumbers[i+1]);
		}
		printf("\n");
	}
	fflush(stdout);
}

void printPuzzle(const Puzzle& prob,Board& sol, Board &sol2){
	for(unsigned i = 0; i < P_SIZE; i++){
		for(unsigned j = 0;j < P_SIZE; j++){
      if ( getSquare(j, i, sol) != getSquare(j, i, sol2) ) 
      {
        if ( getSquare(j, i, sol) == SQUARE_BLOCK )
          printf( "¢Ð") ;
        else if ( getSquare(j, i, sol) == SQUARE_SPACE ) 
          printf( "¢å" );
      }
      else
      {
		  	switch (getSquare(j, i, sol) )
		  	{
		  	case SQUARE_BLOCK: 
			  	printf("¡½");
			  	break;
		  	case SQUARE_SPACE: 
		  		printf("¢®");
			  	break;
		  	case SQUARE_UNKNOWN:
		  		printf("¡¼");
		  		break;
		  	default:
			  	printf("¦ä");
			  	break;
		  	}
      }
			
		}
		for(unsigned j = 0; j <= prob.mLines[ i + P_SIZE ].mCount; j++){
			if(prob.mLines[ i + P_SIZE ].mNumbers[ j ] != 0)
	   		printf("%2u ", prob.mLines[ i + P_SIZE ].mNumbers[ j ]);
		}
		printf("\n",i);
	}

	for(unsigned i = 1; i <= P_SIZE; i = i++ ){
		for(unsigned j = 0 ; j < P_SIZE; j++){
      if ( prob.mLines[j].mNumbers[i] > 0 && i <= prob.mLines[j].mCount)
			  printf("%2d",prob.mLines[j].mNumbers[i]);
		  else
        printf("  ");
			
		}
		printf("\n");
	}
	fflush(stdout);
}


void readBuffer(Puzzle& quest,const char* sInput){
#ifdef _DEBUG
	for(unsigned i = 0 ; i < P_SIZE * 2 ;i++){
		quest.mLines[i].mCount = 0;
	}
#endif
	char *token;
	int count = 0;
	char buffer[1300];
	strcpy(buffer,sInput);
	for( token = strtok(buffer, "z"), count = 0; token; token = strtok( NULL, "z"), count++ ){
		quest.mLines[count].mCount = (unsigned char)strlen(token);
		quest.mLines[count].mNumbers[0] = 0;
		quest.mLines[count].mSum[0] = 0;
		quest.mLines[count].hashKey = 0;
    int tmpIndex = 0 ;
	  int sum = 0;
		for(unsigned i = 1;i <= strlen(token); i++){
			quest.mLines[count].mNumbers[i] = (unsigned)(token[i-1]) - (unsigned)'a' + 1;
			sum += quest.mLines[count].mNumbers[i];
			quest.mLines[count].mSum[i] = sum;
      if ( quest.mLines[count].mNumbers[i] )
      {
		 
        quest.mLines[count].hashKey ^= zHashKeyTable[tmpIndex][quest.mLines[count].mNumbers[i]];
        tmpIndex++ ;
      }
		}
    //printf("%llu\n", quest.mLines[count].hashKey);
    //printf("%d\n", quest.mLines[count].mSum[quest.mLines[count].mCount] ) ;
    //printf("%llu\n", quest.mLines[count].hashKey);
	}
	delete token;
	token = NULL;
}


void printBoardTaai(const Board& sol){
	for(unsigned i = 0; i < P_SIZE; i++){
		for(unsigned j = 0;j < P_SIZE; j++){
			switch (getSquare(j, i, sol) )
			{
			case SQUARE_BLOCK: 
				printf("1");
				break;
			case SQUARE_SPACE: 
				printf("0");
				break;
			case SQUARE_UNKNOWN:
				printf("-1");
				break;
			default:
				printf("-2");
				break;
			}
			if( j != P_SIZE-1) printf("\t");
			else printf("\n");
		}
	}
	fflush(stdout);
}