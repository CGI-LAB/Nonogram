#ifndef _PUZZLE_H_
#define _PUZZLE_H_

#include "config.h"
#include "bit.h"
#include "lineMask.h"
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

struct OneCell
{
	int squareType ;
	int x ;
	int y ;
};

// 2012.07.12 by derjohng.
// 為了減少 probing 時的 overhead..
struct ProbingQueue
{
public:
	bool inQ[S_SIZE+1] ;
	OneCell dataQ[S_SIZE+1] ;
	unsigned int f_pos ;
	unsigned int r_pos ;
	unsigned int max_size ;

public:
	inline void initial()
	{
		memset( inQ, 0, sizeof(inQ) );
		f_pos = 0 ;
		r_pos = 0 ;
		max_size = S_SIZE+1;
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

	inline void pushQ(int pX, int pY)
	{
		if(inQ[pX*P_SIZE+pY]) 
			return;
		dataQ[r_pos].x = pX;
		dataQ[r_pos].y = pY;
		r_pos = ( r_pos + 1 ) % max_size ;
		setInQ(pX, pY, true);
	}

	inline OneCell popAndFront()
	{
		OneCell uFront = dataQ[f_pos];
		f_pos = ( f_pos + 1 ) % max_size ;
		setInQ(uFront.x, uFront.y, false);
		return uFront;
	}

private:
	inline bool isInQ(int pX, int pY)
	{
		return inQ[pX*P_SIZE+pY];
	}

	inline void setInQ(int pX, int pY,bool value)
	{
		inQ[pX*P_SIZE+pY] = value;
	}



};

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
		if (isEmpty())
		{
			cerr << "Should not Empty in popAndFront()" << endl;
			exit(1);
		}
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

#ifdef DEBUG_TREENODES
	inline void debug()
	{
		printf(" hit:%llu query:%llu ratio:%.2f\n", totalHit, totalQuery, ((float) totalHit/totalQuery));
		printf(" lasthit:%llu lastquery:%ull\n", totalHit-lastHit, totalQuery-lastQuery);
		lastQuery = totalQuery;
		lastHit   = totalHit;
	}
#endif
#ifdef ENABLE_BACKJUMPING
	inline void debugJumpTable()
	{
		for (int j=0; j<P_SIZE; ++j)
		{
			for (int i=0; i<P_SIZE; ++i)
			{
				// cout << jumpLevels[i][j] << " ";
				printf("%02d ", jumpLevels[i][j]);
			}
			cout << endl;
		}
		return;
	}
#endif
public:
	bool inQ[P_SIZE * 2] ;
	int dataQ[P_SIZE * 2 + 1] ;
	unsigned int f_pos ;
	unsigned int r_pos ;
	unsigned int max_size ;
	unsigned long long totalQuery;
	unsigned long long totalHit;
	unsigned long long lastQuery;
	unsigned long long lastHit;
	unsigned long long totalProbing;
	unsigned long long totalMerging;
	unsigned long long totalPaintedByLineSolver;
	unsigned long long layerNodeCount;
	double layerAverage;
	int lastX;
	int lastY;
	int lastColor;
#ifdef ENABLE_BACKJUMPING
	unsigned short jumpLevels[P_SIZE][P_SIZE];
	int targetJump;

	int jumpSelectX;
	int jumpSelectY;
	int jumpSelectColor;
#endif
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

struct NodeQueue
{
public:
  static MyQueue myQ ;
  static ProbingQueue pQ;
} ;

struct Point 
{
  unsigned char x ;
  unsigned char y ;
  int squareType ;
} ;

struct UpdateArea
{
  Point points[S_SIZE] ; 
  int num ;
} ;

// Sizeof( Board ) = 408 bytes
struct Board
{
  short numOfPaintedSquares ;
#ifdef SSE_BOARD
	LineMask_128 colString[P_SIZE];
	LineMask_128 rowString[P_SIZE];
#else
	LineMask colString[P_SIZE];
	LineMask rowString[P_SIZE];
#endif
   UpdateArea updateArea ;
   short lastColor; // 最後猜點顏色
};

inline void copyBoardWithoutUpdate(Board &dstBoard, Board &srcBoard)
{
	memcpy(dstBoard.colString, srcBoard.colString, sizeof(dstBoard.colString));
	memcpy(dstBoard.rowString, srcBoard.rowString, sizeof(dstBoard.rowString));
}

struct ShortBoard
{
	short numOfPaintedSquares ;
#ifdef SSE_BOARD
	LineMask_128 colString[P_SIZE];
	LineMask_128 rowString[P_SIZE];
#else
	LineMask colString[P_SIZE];
	LineMask rowString[P_SIZE];
#endif
};

class UpdateList
{
public:
  Point points[S_SIZE * 2] ; 
  bool inQ[2*S_SIZE + 1]; 
  int num ;
  int lastnum;
  bool listUpdated;

  UpdateList()
  {
	  memset(inQ, 0, sizeof(inQ));
  }

  inline void clear()
  {
	  for (int i=0; i<num; ++i)
	  {
		setQ(points[i].x, points[i].y, points[i].squareType, false);
	  }
	  num = 0;
	  lastnum = 0;
	  listUpdated = false;
	  
  }

  inline void pushQ(int pX, int pY, int pS)
  {
	  if (isInQ(pX, pY, pS)) return;
	  int index = num ;
	  points[index].x = pX ;
	  points[index].y = pY ;
	  points[index].squareType = pS ; // 反向填白
	  num++ ;
	  listUpdated = true;

	  setQ(pX, pY, pS, true);
  }

  inline bool isInQ(int pX, int pY, int pS)
  {
	  return inQ[(pS-1)*S_SIZE + pX*P_SIZE + pY];
  }

  inline void setQ(int pX, int pY, int pS, bool pValue)
  {
	  inQ[(pS-1)*S_SIZE + pX*P_SIZE + pY] = pValue;
  }
};

struct PresearchBoard
{
  Board board ;
  UpdateList updateList ;
} ;


inline void setSquare(int x, int y, Board& sol, int iSet)
{
	//kk: this can be modified to much more easy statement.
#ifdef SSE_BOARD
	sol.colString[x] &= ~SQUARE_MASK[ iSet ][ y ];
	sol.rowString[y] &= ~SQUARE_MASK[ iSet ][ x ];
#else
	sol.colString[x] &= ~(SHIFT_L(iSet,y));
	sol.rowString[y] &= ~(SHIFT_L(iSet,x));
#endif
}

inline void setLine(int x, int y, void *line, int iSet1, int iSet2)
{
	//kk: this can be modified to much more easy statement.
#ifdef SSE_BOARD
	(LineMask_128 *) line &= ~SQUARE_MASK[ iSet ][ y ];
	(LineMask_128 *) line &= ~SQUARE_MASK[ iSet ][ x ];
#else
	LineMask *qLine = (LineMask *) line;
	(*qLine) &= ~(SHIFT_L(iSet2,y));
	(*qLine) &= ~(SHIFT_L(iSet1,x));
#endif
}


//     ■■■■□□□□■■╳■■╳■
#define SQUARE_BLOCK 1 //■  ■
#define SQUARE_SPACE 2 //╳  ╳
#define SQUARE_UNKNOWN 3 //□  □
#define SQUARE 3

inline int getSquare(int x, int y, const Board& sol){
#ifdef SSE_BOARD
	return sol.rowString[y][x];
#else
	return (int)( SHIFT_R(sol.rowString[y] , x) & SQUARE );
#endif
}

inline void initBoard(Board& sol)
{
  memset(&sol, 0xff, sizeof(Board));
  sol.numOfPaintedSquares = 0 ;
  
  FILE* input = fopen( INPUT_STATE, "r" ) ;
  if ( input != NULL )
  {
    for ( int i = 0 ; i < P_SIZE ; i++ )
    {
      for ( int j = 0 ; j < P_SIZE ; j++ )
      {
        char c ;
        fscanf( input, "%c ", &c ) ;
        if ( c == 'W' ){
          setSquare(j, i, sol, SQUARE_BLOCK);
          sol.numOfPaintedSquares++ ;
          }
        else if ( c == 'B' ) {
          setSquare(j, i, sol, SQUARE_SPACE);
          sol.numOfPaintedSquares++ ;
          }
        //else if ( c == '.' )
        //  setSquare(j, i, sol, SQUARE_UNKNOWN);
       
      }
    }
  }

  NodeQueue::myQ.initial() ;
  //system( "pause" ) ;
  memset( &sol.updateArea, 0, sizeof( UpdateArea ) ) ;
  for ( int i = 0 ; i < P_SIZE * 2 ;i++ )
		NodeQueue::myQ.pushQ(i) ;

}

void printPuzzle(const Puzzle& quest,Board& sol);
void printPuzzle(const Puzzle& prob,Board& sol, Board &sol2) ;
void printBoardTaai(const Board& sol);

int countPaintedCells(Board& sol);

void readBuffer(Puzzle& quest,const char* sInput);

inline void setButNoFlag(int i, int j,Board& sol,int toSet)
{
	if ( getSquare( i, j, sol ) == SQUARE_UNKNOWN ) 
	{
		sol.numOfPaintedSquares++ ;
		setSquare(i,j,sol,toSet);
		int updateIndex = sol.updateArea.num ;
		sol.updateArea.points[updateIndex].x = i ;
		sol.updateArea.points[updateIndex].y = j ;
		sol.updateArea.points[updateIndex].squareType = toSet ;
		sol.updateArea.num++ ;
	}
	// NodeQueue::myQ.pushQ(i);
	// NodeQueue::myQ.pushQ(j+P_SIZE);
};


inline void setAndFlag(int i, int j,Board& sol,int toSet)
{
	/* if ( getSquare( i, j, sol ) == SQUARE_UNKNOWN ) 
	{
		sol.numOfPaintedSquares++ ;
		setSquare(i,j,sol,toSet);
		int updateIndex = sol.updateArea.num ;
		sol.updateArea.points[updateIndex].x = i ;
		sol.updateArea.points[updateIndex].y = j ;
		sol.updateArea.points[updateIndex].squareType = toSet ;
		sol.updateArea.num++ ;
	}*/
	setButNoFlag(i, j, sol, toSet);
	NodeQueue::myQ.pushQ(i);
	NodeQueue::myQ.pushQ(j+P_SIZE);
};
#endif
