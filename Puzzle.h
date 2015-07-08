#ifndef _PUZZLE_H_
#define _PUZZLE_H_

#include "config.h"
#include "bit.h"
#include <cstring>
#include <stdio.h>

// Sizeof( MyQueue ) = 268 bytes
struct MyQueue 
{
public:
	inline void initial()
	{
		memset( inQ, 0, sizeof(inQ) );
		f_pos = 0 ;
		r_pos = 0 ;
		max_size = P_SIZE * 2  + 1 ;
	}

	inline bool isInQ(int index)
	{
		return inQ[index];
	}

	inline void setInQ(int index,bool value)
	{
		inQ[index] = value;
	}
	
	inline void pushQ(int index)
	{
        if(inQ[index]) 
            return;
		dataQ[r_pos] = index ;
		r_pos = ( r_pos + 1 ) % max_size ;
		setInQ(index, true);
	}

	inline unsigned char popAndFront()
	{
		unsigned char uFront = dataQ[f_pos];
		f_pos = ( f_pos + 1 ) % max_size ;
		setInQ(uFront,false);
		return uFront;
	}

	inline bool isEmpty()
	{
		return r_pos == f_pos ;
	}
    inline void clear()
    {
        memset( inQ, 0, sizeof(inQ) );
        f_pos = 0 ;
		r_pos = 0 ;
    }
public:
	bool inQ[P_SIZE * 2] ;
	int dataQ[P_SIZE * 2 + 1] ;
	unsigned int f_pos ;
	unsigned int r_pos ;
	unsigned int max_size ;
};

// Sizeof( LineNumbers ) = 64 bytes
struct LineNumbers{
	unsigned char mCount;
	unsigned char mNumbers[ P_SIZE + 2 ];
	unsigned char mSum[ P_SIZE + 2 ];
	LineMask hashKey ;	
};

// Sizeof( Puzzle ) = 3200 bytes
struct Puzzle{
	Puzzle();
	Puzzle(const char *);
	~Puzzle();
	LineNumbers mLines[ 2 * P_SIZE ];
};

class NodeQueue
{
public:
  static MyQueue myQ ;
} ;

// Sizeof( Board ) = 408 bytes
struct Board{
  short numOfSquareOnBoard ;
	LineMask colString[P_SIZE];
	LineMask rowString[P_SIZE];
  //LinkedList unknownList ;
};


inline void setSquare(int x, int y, Board& sol, LineMask iSet){
	sol.colString[x] &= ~(SHIFT_L(iSet,y));
	sol.rowString[y] &= ~(SHIFT_L(iSet,x));
}


//     ――――ーーーー――〓――〓―
#define SQUARE_BLOCK 1 //―  ―
#define SQUARE_SPACE 2 //〓  〓
#define SQUARE_UNKNOWN 3 //ー  ー
#define SQUARE 3

inline int getSquare(int x, int y, const Board& sol){
	return (int)( SHIFT_R(sol.rowString[y] , x) & SQUARE );
}

inline void initBoard(Board& sol){
	memset(&sol, 0xff, sizeof(Board));
  sol.numOfSquareOnBoard = 0 ;
  NodeQueue::myQ.initial() ;
  //system( "pause" ) ;
    for ( int i = 0 ; i < P_SIZE * 2 ;i++ )
		NodeQueue::myQ.pushQ(i) ;

}

void printPuzzle(const Puzzle& quest,Board& sol);

void printBoardTaai(const Board& sol);

void readBuffer(Puzzle& quest,const char* sInput);


inline void setAndFlag(int i, int j,Board& sol,int toSet){
	setSquare(i,j,sol,toSet);
  sol.numOfSquareOnBoard++ ;
	NodeQueue::myQ.pushQ(i);
	NodeQueue::myQ.pushQ(j+P_SIZE);

};


#endif