#include "puzzle.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>

extern LineMask zHashKeyTable[13][26];

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

void printPuzzle(const Puzzle& prob,Board& sol){
	for(unsigned i = 0; i < P_SIZE; i++){
		for(unsigned j = 0;j < P_SIZE; j++){
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
		for(unsigned j = 0; j < prob.mLines[ i + P_SIZE ].mCount; j++){
			if(prob.mLines[ i + P_SIZE ].mNumbers[ j ] != 0)
			printf("%2u ", prob.mLines[ i + P_SIZE ].mNumbers[ j ]);
		}
		printf("\n",i);
	}
	for(unsigned i = 1; i < P_SIZE; i+=2){
		for(unsigned j = 0;j < P_SIZE; j++){
			if(i >= prob.mLines[j].mCount){
				printf("  ");
				continue;
			}
			printf("%2d",prob.mLines[j].mNumbers[i]);
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
}