#include "LineSolver.h"
#include "config.h"
#include "bit.h"
#include <cstdio>
#include <iostream>
#include <algorithm>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#include "Dependency.h"


#include "Hash.h"


using namespace std;


unsigned char DPtable[P_SIZE+1][(P_SIZE+1)/2+1];
extern LineMask_128 SEGMENT[P_SIZE + 1][P_SIZE + 1];


#ifdef SSE_BOARD
int lineSolver(const Puzzle& prob, Board& sol, const bool reetUpdateAreaFlag ){

    int state = UNSOLVED;

    if ( reetUpdateAreaFlag )
      sol.updateArea.num = 0 ;

    while(!NodeQueue::myQ.isEmpty())
	{
        unsigned char i = NodeQueue::myQ.popAndFront();
		NodeQueue::myQ.totalQuery += 1;

		// dbTable.thisTempTable.setBit( i );//|= ( 1ULL << i );
        
        /*solved if flags*/
        LineMask_128 settle;
        if(!findHash(prob.mLines[i], *(sol.colString+i),settle))
        {
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            if( false == sprintSettle( prob.mLines[i], 	*(sol.colString + i), P_SIZE, prob.mLines[i].mCount ,  settle) )
            {   
               // insertHash(prob.mLines[i], *(sol.colString+i),CONFLICT);
                NodeQueue::myQ.clear();
                return CONFLICT;
            }
            insertHash(prob.mLines[i], *(sol.colString+i),settle);
		} else {
			NodeQueue::myQ.totalHit += 1;
		}
    //    if(settle == CONFLICT ) return CONFLICT;

		
	    LineMask_128 change =  (sol.colString[i] ^ settle); // 這段黑變白,白變黑. (但白即白, 黑即黑)
        for(int j=0; j<P_SIZE; j++)
        {
			//kk: after then, how to stop this iteration eralier if there is all "0" in index that > j in change
			//int n = change[j];
            if( change[j] )/*& UNKNOWN_SQUARE_MASK[ j ]) != 0 )*/
            {
                sol.numOfPaintedSquares++ ;
                if(i < P_SIZE)	{
#ifdef DOING_INVERSE_LINKS
					int updateIndex = sol.updateArea.num ;
					sol.updateArea.points[updateIndex].x = i ;
					sol.updateArea.points[updateIndex].y = j ;
					sol.updateArea.points[updateIndex].squareType = change[j] ;
					sol.updateArea.num++ ;
#endif

					setSquare(i,j,sol,change[j]); 
					NodeQueue::myQ.pushQ( j + P_SIZE ); 
				}
                else { 
#ifdef DOING_INVERSE_LINKS
					int updateIndex = sol.updateArea.num ;
					sol.updateArea.points[updateIndex].x = j ;
					sol.updateArea.points[updateIndex].y = i-P_SIZE ;
					sol.updateArea.points[updateIndex].squareType = change[j] ;
					sol.updateArea.num++ ;
#endif

					setSquare(j,i-P_SIZE,sol,change[j]); 
					NodeQueue::myQ.pushQ( j ); 
				}	
            }
        }
        //	printPuzzle(prob,sol);
        //	printf("i = %d\n", i);
    } //end countinue run

    if ( sol.numOfPaintedSquares == S_SIZE )
	{
        return SOLVED;
	}
    else
        return UNSOLVED ;
}

#else
int lineSolver(const Puzzle& prob, Board& sol, const bool reetUpdateAreaFlag ){

    int state = UNSOLVED;

    if ( reetUpdateAreaFlag )
      sol.updateArea.num = 0 ;
    while (!NodeQueue::myQ.isEmpty())
	{
        unsigned char i = NodeQueue::myQ.popAndFront();
		NodeQueue::myQ.totalQuery += 1;


        /*solved if flags*/
        LineMask settle = 0;
        if(!findHash(prob.mLines[i], *(sol.colString+i),settle))
        {
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            if( false == sprintSettle( prob.mLines[i], 	*(sol.colString+i), P_SIZE, prob.mLines[i].mCount ,  settle) )
            {   
               // insertHash(prob.mLines[i], *(sol.colString+i),CONFLICT);
                NodeQueue::myQ.clear();
                return CONFLICT;
            }
            insertHash(prob.mLines[i], *(sol.colString+i),settle);
        } else {
			NodeQueue::myQ.totalHit += 1;
		}

    //    if(settle == CONFLICT ) return CONFLICT;
        LineMask change =  (sol.colString[i] ^ settle);
        for(int j=0;change!=0&&j<P_SIZE;change>>=2,j++)
        {
            LineMask temp = (change & 3);
            if(temp)
            {
                sol.numOfPaintedSquares++ ;
				NodeQueue::myQ.totalPaintedByLineSolver += 1;

                if(i < P_SIZE)
                { 
#ifdef DOING_INVERSE_LINKS
                    int updateIndex = sol.updateArea.num ;
					sol.updateArea.points[updateIndex].x = i ;
					sol.updateArea.points[updateIndex].y = j ;
					sol.updateArea.points[updateIndex].squareType = (int) temp ;
					sol.updateArea.num++ ;
#endif

				    //setSquare(i,j,sol,change[j]); 
					setSquare(i,j,sol,(int) temp); 
					NodeQueue::myQ.pushQ( j + P_SIZE );
                }
                else 
				{
#ifdef DOING_INVERSE_LINKS
					int updateIndex = sol.updateArea.num ;
					sol.updateArea.points[updateIndex].x = j ;
					sol.updateArea.points[updateIndex].y = i-P_SIZE ;
					sol.updateArea.points[updateIndex].squareType = (int) temp ;
					sol.updateArea.num++ ;
#endif

					setSquare(j,i-P_SIZE,sol,(int) temp); 
					NodeQueue::myQ.pushQ( j ); 
				}	
            }
        }
        //	printPuzzle(prob,sol);
        //	printf("i = %d\n", i);
    } //end countinue run

    if ( sol.numOfPaintedSquares == S_SIZE )
        return SOLVED;
    else
        return UNSOLVED ;
}
#endif


static LineMask blockMask[] = {
    0,
    0x1ull,
    0x5ull,
    0x15ull,
    0x55ull,
    0x155ull,
    0x555ull,
    0x1555ull,
    0x5555ull,
    0x15555ull,
    0x55555ull,
    0x155555ull,
    0x555555ull,
    0x1555555ull,
    0x5555555ull,
    0x15555555ull,
    0x55555555ull,
    0x155555555ull,
    0x555555555ull,
    0x1555555555ull,
    0x5555555555ull,
    0x15555555555ull,
    0x55555555555ull,
    0x155555555555ull,
    0x555555555555ull,
    0x1555555555555ull,
	0x5555555555555ull,
	0x15555555555555ull,
	0x55555555555555ull,
	0x155555555555555ull,
	0x555555555555555ull,
	0x1555555555555555ull,
	0x5555555555555555ull,
};

/*this fucntion is recursively settle with a single line.*/
#ifdef SSE_BOARD
bool sprintSettle(
    const LineNumbers& des,
    LineMask_128 inString, 
    int i, 
    int j, 
    LineMask_128& settleString, 
    unsigned char* Dtable)
{
    if( ( i == -1 || i == 0 ) && j == 0) return true;
    if(i < 0){ 
        return false;
    }
    if(DPtable[i][j] == CONFLICT) return false;
    if(DPtable[i][j] == SOLVED) return true;

    int nowSquare = inString[i - 1]; //SQUARE & SHIFT_R(inString , i-1 ) ;

    int positionOfBlock = i - des.mNumbers[j];
    if(positionOfBlock < 0){
        DPtable[i][j] = CONFLICT;
        return false;
    }

    LineMask_128 thisBlockMask = SEGMENT[ positionOfBlock ][ positionOfBlock + des.mNumbers[j] - 1 ];
	if(positionOfBlock - 1 >= 0) thisBlockMask |= WHITE_SQUARE_MASK[ positionOfBlock - 1 ];
	//	for( int k = positionOfBlock; k < positionOfBlock + des.mNumbers[j]; k++ )
	//		thisBlockMask |= BLACK_SQUARE_MASK[k];
	//SHIFT_L( blockMask[des.mNumbers[j]], positionOfBlock ) | 
	//SHIFT_L(SQUARE_SPACE, positionOfBlock - 1 );

    bool isValid = false;
    bool shouldTryBlock = true ;
    if( (thisBlockMask & (~inString)) != ZERO ) 
        shouldTryBlock =  false; // if there is an space on the n next square, then we don't try block.

    if( nowSquare != SQUARE_SPACE && shouldTryBlock && j ){
        if(true == sprintSettle(des, inString, positionOfBlock -1, j - 1, settleString, Dtable)){ // set this n square as block		
            settleString |= thisBlockMask;
            isValid = true;
        }
	  }

    if( nowSquare != SQUARE_BLOCK &&	( i  > des.mSum[j] ) ){
        if(true == sprintSettle(des, inString, i - 1 , j , settleString, Dtable)){ //set this 1 square as space
            settleString |= WHITE_SQUARE_MASK[ i - 1 ];
            isValid = true;
        }
	  }
    
	DPtable[i][j] = (isValid)? SOLVED : CONFLICT;
    return isValid;
}
#else
bool sprintSettle(
    const LineNumbers& des,
    LineMask inString, 
    int i, 
    int j, 
    LineMask& settleString, 
    unsigned char* Dtable)
{
    if( ( i == -1 || i == 0 ) && j == 0) return true;
    if(i < 0){ 
        return false;
    }
    if(DPtable[i][j] == CONFLICT) return false;
    if(DPtable[i][j] == SOLVED) return true;

    int nowSquare = SQUARE & SHIFT_R(inString , i-1 ) ;

    int positionOfBlock = i - des.mNumbers[j];
    if(positionOfBlock < 0){
        DPtable[i][j] = CONFLICT;
        return false;
    }
    LineMask thisBlockMask = SHIFT_L( blockMask[des.mNumbers[j]], positionOfBlock ) | SHIFT_L(SQUARE_SPACE, positionOfBlock - 1 );

    bool isValid = false;
    bool shouldTryBlock = true ;
    if( thisBlockMask & (~inString)  ) 
        shouldTryBlock =  false; // if there is an space on the n next square, then we don't try block.

    if( nowSquare != SQUARE_SPACE && shouldTryBlock && j )
        if(true == sprintSettle(des, inString, positionOfBlock -1, j - 1, settleString, Dtable)){ // set this n square as block		

            settleString |= thisBlockMask;
 
            isValid = true;
        }
        if( nowSquare != SQUARE_BLOCK &&	( i  > des.mSum[j] ) )
            if(true == sprintSettle(des, inString, i - 1 , j , settleString, Dtable)){ //set this 1 square as space
                settleString |= SHIFT_L(SQUARE_SPACE, (i-1));
                isValid = true;
            }
            DPtable[i][j] = (isValid)? SOLVED : CONFLICT;
            return isValid;
}
#endif
