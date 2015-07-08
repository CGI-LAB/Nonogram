#include "lineSolver.h"
#include "config.h"
#include "bit.h"
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <intrin.h>
#include "Dependency.h"


#include "Hash.h"


using namespace std;


unsigned char DPtable[P_SIZE+1][(P_SIZE+1)/2];
extern DependencyTable dbTable;


int lineSolver(const Puzzle& prob, Board& sol){

    int state = UNSOLVED;
    while(!NodeQueue::myQ.isEmpty()){
        unsigned char i = NodeQueue::myQ.popAndFront();

         
         dbTable.thisTempTable |= ( 1ULL << i );
        
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
        }
    //    if(settle == CONFLICT ) return CONFLICT;
        LineMask change =  (sol.colString[i] ^ settle);
        for(int j=0;change!=0&&j<P_SIZE;change>>=2,j++)
        {
            LineMask temp = (change & 3);
            if(temp)
            {
                sol.numOfSquareOnBoard++ ;
                if(i < P_SIZE)	{ setSquare(i,j,sol,temp); NodeQueue::myQ.pushQ( j + P_SIZE ); }
                else { setSquare(j,i-P_SIZE,sol,temp); NodeQueue::myQ.pushQ( j ); }	
            }
        }
        //	printPuzzle(prob,sol);
        //	printf("i = %d\n", i);
    } //end countinue run

    if ( sol.numOfSquareOnBoard == S_SIZE )
        return SOLVED;
    else
        return UNSOLVED ;
}



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
};
static LineMask newBlockMask[] = {
    0x2ull,
    0x6ull,
    0x16ull,
    0x56ull,
    0x156ull,
    0x556ull,
    0x1556ull,
    0x5556ull,
    0x15556ull,
    0x55556ull,
    0x155556ull,
    0x555556ull,
    0x1555556ull,
    0x5555556ull,
    0x15555556ull,
    0x55555556ull,
    0x155555556ull,
    0x555555556ull,
    0x1555555556ull,
    0x5555555556ull,
    0x15555555556ull,
    0x55555555556ull,
    0x155555555556ull,
    0x555555555556ull,
    0x1555555555556ull,
    0x5555555555556ull,
    0x15555555555556ull,
    0x55555555555556ull,
};
/*this fucntion is recursively settle with a single line.*/
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