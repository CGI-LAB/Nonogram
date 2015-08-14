#include <queue>
#include <cstdio>
#include <iostream>

#include "SearchSolver.h"
#include "LineSolver.h"
#include "Dt.h"


static PresearchBoard ifGuessBlackBoards[P_SIZE][P_SIZE] ;
static PresearchBoard ifGuessWhiteBoards[P_SIZE][P_SIZE] ;

// 2012.07.15 probing boards 會儲存下來, backtracing 回來時,會拷貝來用
// 3 變數分別是 depth, i, j.
// 2012.07.22, SSE 下會超過 static stack 限制
#ifndef SSE_BOARD
static ShortBoard fullyProbingBlackBoards[S_SIZE][P_SIZE][P_SIZE];
static ShortBoard fullyProbingWhiteBoards[S_SIZE][P_SIZE][P_SIZE];
#endif

static int numberPaintedInBlackBoards[P_SIZE][P_SIZE];
static int numberPaintedInWhiteBoards[P_SIZE][P_SIZE];


// DependencyTable dbTable;
extern int currentId ;
extern int gLimitedLevel ;
extern int gNumOfWhiteConflict ;
extern int gNumOfBlackConflict ;
extern int gNumOfMerge ;
extern int gNumOfNoop ;
extern int gDiff ;

// 20120516, 減少 ifGuessBlackBoard, ifGuessWhiteBoard 之 copy..
inline bool mergeBoard( Board &dstBoard, Board &srcBoard, bool putQueue)
{
	// dstBoard = srcBoard;
	// return false;
	int painted = 0;
	int tmpcount = 0;

	for ( int i = 0 ; i < P_SIZE ; ++i )
	{
#ifdef SSE_BOARD
		LineMask_128 oldRow = dstBoard.rowString[i] ;
		LineMask_128 oneRow = srcBoard.rowString[i] & oldRow;
#else
		LineMask oldRow = dstBoard.rowString[i];
		LineMask oneRow = srcBoard.rowString[i] & oldRow;
		
#endif
		if ( oneRow != oldRow )
		{
			// detect 00 here..
#ifdef SSE_BOARD
			// LineMask_128 tmp = oneRow ;

			LineMask tmp = oneRow.mBit.n[0] ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..

			tmp = oneRow.mBit.n[1] ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
						
#else
			LineMask tmp = oneRow ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
#endif
			

			dstBoard.rowString[i] = oneRow;
			
#ifdef SSE_BOARD
			tmpcount = count128( oneRow ^ oldRow ) ;
#else
			tmpcount = count64( oneRow ^ oldRow  ) ;
#endif
			painted += tmpcount;

			if (oneRow!=srcBoard.rowString[i] && oneRow!=oldRow) NodeQueue::myQ.pushQ(i + P_SIZE);
			else if (putQueue && tmpcount>0) NodeQueue::myQ.pushQ(i + P_SIZE);

		}

#ifdef SSE_BOARD
		LineMask_128 oldCol = dstBoard.colString[i] ;
		LineMask_128 oneCol = srcBoard.colString[i] & oldCol;
#else
		LineMask oldCol = dstBoard.colString[i];
		LineMask oneCol = srcBoard.colString[i] & oldCol;
#endif
		if ( oneCol != oldCol )
		{
			// detect 00 here..
#ifdef SSE_BOARD
			LineMask tmp = oneCol.mBit.n[0] ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..

			tmp = oneCol.mBit.n[1] ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
#else
			LineMask tmp = oneCol ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
#endif
			

			dstBoard.colString[i] = oneCol;

			if (oneCol!=srcBoard.colString[i] && oneCol!=oldCol) NodeQueue::myQ.pushQ(i);
			else if (putQueue)
			{
#ifdef SSE_BOARD
				tmpcount = count128( oneCol ^ oldCol  ) ;
#else
				tmpcount = count64( oneCol ^ oldCol  ) ;
#endif
				if (tmpcount>0) NodeQueue::myQ.pushQ(i);
			}

			// PS: 下一個 function 也要改

		}
	} // for each line

	dstBoard.numOfPaintedSquares += painted;

	return false;
}

// 2012.07.22, SSE 下會超過 static stack 限制
#ifndef SSE_BOARD
inline bool mergeBoard( Board &dstBoard, ShortBoard &srcBoard, bool putQueue)
{
	// dstBoard = srcBoard;
	// return false;
	int painted = 0;
	int tmpcount;

	for ( int i = 0 ; i < P_SIZE ; ++i )
	{
#ifdef SSE_BOARD
		LineMask_128 oldRow = srcBoard.rowString[i] ;
		LineMask_128 oneRow = srcBoard.rowString[i] & oldRow;
#else
		LineMask oldRow = dstBoard.rowString[i];
		LineMask oneRow = srcBoard.rowString[i] & oldRow;

#endif
		if ( oneRow != oldRow )
		{
			// detect 00 here..
#ifdef SSE_BOARD
			LineMask_128 tmp = oneRow ;
			for (int qq=0; qq<P_SIZE; ++qq)
				if (oneRow[qq]==0) return true; // conflict..
#else
			LineMask tmp = oneRow ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
#endif
			

			dstBoard.rowString[i] = oneRow;

#ifdef SSE_BOARD
			tmpcount += count128( oneRow ^ oldRow ) ;
#else
			tmpcount = count64( oneRow ^ oldRow  ) ;
#endif
			painted += tmpcount;

			if (oneRow!=srcBoard.rowString[i] && oneRow!=oldRow) NodeQueue::myQ.pushQ(i + P_SIZE);
			else if (putQueue && tmpcount>0) NodeQueue::myQ.pushQ(i + P_SIZE);

		}

#ifdef SSE_BOARD
		LineMask_128 oldCol = srcBoard.colString[i] ;
		LineMask_128 oneCol = srcBoard.colString[i] & oldCol;
#else
		LineMask oldCol = dstBoard.colString[i];
		LineMask oneCol = srcBoard.colString[i] & oldCol;
#endif
		if ( oneCol != oldCol )
		{
			// detect 00 here..
#ifdef SSE_BOARD
			LineMask_128 tmp = oneCol;
			for (int qq=0; qq<P_SIZE; ++qq)
				if (oneCol[qq]==0) return true; // conflict..
#else
			LineMask tmp = oneCol ;
			tmp = tmp | (tmp >>1) | 0xAAAAAAAAAAAAAAAAULL;
			if (~tmp>0) return true; // conflict..
#endif
			

			dstBoard.colString[i] = oneCol;

			if (oneCol!=srcBoard.colString[i] && oneCol!=oldCol) NodeQueue::myQ.pushQ(i);
			else if (putQueue)
			{
#ifdef SSE_BOARD
				tmpcount = count128( oneCol ^ oldCol  ) ;
#else
				tmpcount = count64( oneCol ^ oldCol  ) ;
#endif
				if (tmpcount>0) NodeQueue::myQ.pushQ(i);
			}

			// PS: 下一個 function 也要改

		}
	} // for each line
	dstBoard.numOfPaintedSquares += painted;

	return false;
}
#endif


/////////////////////
// One Probing..
inline int beginOneProbe( const Puzzle& problem,
    Board& correctBoard,  
    PresearchBoard& ifGuessBlackBoard, 
    PresearchBoard& ifGuessWhiteBoard, 
    int x, 
    int y
    ) 
{

/* #ifdef DOING_INVERSE_LINKS
  // 確保有 update 才做!
  if (ifGuessBlackBoard.updateList.listUpdated==false && ifGuessWhiteBoard.updateList.listUpdated==false)
	  return UNSOLVED;
#endif */

  // 拷貝目前盤面,並填 black
  // 猜白.注意: here SQUARE_BLOCK 即是白, SQUARE_SPACE 即是黑! 
  // PS: 和 Line Solver 不一樣
  int state;


  if ( mergeBoard(ifGuessBlackBoard.board, correctBoard, false)==true )
  {
	  // conflict 
	  state = CONFLICT;
  } 
  else
  {
	  // 2012.07.17 用 copy 時,在上層 (x,y) 即已然填過.. 
	  // 所以,以下在 setAndFlag 是不可能再被 enable..
	  NodeQueue::myQ.pushQ(x);
	  NodeQueue::myQ.pushQ(y+P_SIZE);

	  ifGuessBlackBoard.board.updateArea.num = 0 ; 
	  setAndFlag( x, y, ifGuessBlackBoard.board, SQUARE_SPACE ) ; 
	  state = updateInverseList( problem, ifGuessBlackBoard.updateList, ifGuessBlackBoard.board, x, y ) ;
  }

  if ( state == SOLVED )
  {
	  // 解另一解用的
	  mergeBoard(ifGuessWhiteBoard.board, correctBoard, false);
	  setAndFlag( x, y, ifGuessWhiteBoard.board, SQUARE_BLOCK ); 
	  updateInverseList( problem, ifGuessWhiteBoard.updateList, ifGuessWhiteBoard.board, x, y ) ;

	  // main
	  mergeBoard(correctBoard, ifGuessBlackBoard.board, false) ;
	  
	  return SOLVED ;
  }
  else if ( state == CONFLICT )
  {
	  // 2012.07.17 用 copy 時,在上層 (x,y) 即已然填過.. 
	  // 所以,以下在 setAndFlag 是不可能再被 enable..
	  NodeQueue::myQ.pushQ(x);
	  NodeQueue::myQ.pushQ(y+P_SIZE);

	  int state;
	  if ( mergeBoard(correctBoard, ifGuessWhiteBoard.board, false) == true )
	  {
	  	  state = CONFLICT;
	  } else {
		  setAndFlag( x, y, correctBoard, SQUARE_BLOCK ) ;
		  state = updateInverseList( problem, ifGuessWhiteBoard.updateList, correctBoard, x, y ) ;
	  }

	  if ( state == SOLVED )
	  {
		  return SOLVED ;
	  } else if ( state == CONFLICT )
	  {
#ifdef DEBUG_TREENODES
		  cout << "CONFLICT: x:" << x << " y:" << y << " must be WHITE!!"<< endl;
#endif
		  return CONFLICT ;
	  }
	  else
	  {
#ifdef DOING_INVERSE_LINKS
		  sendInverseLinksWhite(ifGuessWhiteBoard, x, y);
		  // sendInverseLinksBlack(ifGuessBlackBoard, x, y);
#endif
		  return CHANGE ;
	  }
  } 
  else // black not conflict.
  {
	  // if (x==0 && y==19) cout << "<--------------here" << endl;
	  // 當猜黑完成,但還沒完成時,這裡再做猜白!

	  int state;
	  if ( mergeBoard( ifGuessWhiteBoard.board, correctBoard, false)==true )
	  {
		  state = CONFLICT;
	  } 
	  else 
	  {
		  // 2012.07.17 用 copy 時,在上層 (x,y) 即已然填過.. 
		  // 所以,以下在 setAndFlag 是不可能再被 enable..
		  NodeQueue::myQ.pushQ(x);
		  NodeQueue::myQ.pushQ(y+P_SIZE);

		  ifGuessWhiteBoard.board.updateArea.num = 0 ;
		  setAndFlag( x, y, ifGuessWhiteBoard.board, SQUARE_BLOCK ) ;

		  // cerr << "Step 3" << endl;
		  state = updateInverseList( problem, ifGuessWhiteBoard.updateList, ifGuessWhiteBoard.board, x, y ) ;
	  }
	 

		if ( state == SOLVED )
		{
			mergeBoard(correctBoard, ifGuessWhiteBoard.board, false) ;

			// 解另一解用的
			setAndFlag( x, y, ifGuessBlackBoard.board, SQUARE_BLOCK ); 
			updateInverseList( problem, ifGuessBlackBoard.updateList, ifGuessBlackBoard.board, x, y ) ;
			return SOLVED ;
		}
		else if ( state == CONFLICT )
		{
			// initBoard( ifGuessWhiteBoard.board );
#ifdef DEBUG_TREENODES
			cout << "CONFLICT2: x:" << x << " y:" << y << " must be BLACK!!" << endl;
			if (x==7 && y==12)
				printPuzzle(problem, ifGuessBlackBoard.board);
#endif

			mergeBoard(correctBoard, ifGuessBlackBoard.board, false) ;		

#ifdef DOING_INVERSE_LINKS
			// sendInverseLinksWhite(ifGuessWhiteBoard, x, y);
			sendInverseLinksBlack(ifGuessBlackBoard, x, y);
#endif
			return CHANGE ;     
		} 
		else
		{ 

#ifdef DOING_INVERSE_LINKS
			sendInverseLinksWhite(ifGuessWhiteBoard, x, y);
			sendInverseLinksBlack(ifGuessBlackBoard, x, y);
#endif

			// 2012.04.10 by derjohng.
			// Merge Operation here.., no need this one if inverse link implemented.
			bool isUpdate = false ;
			for ( int i = 0 ; i < P_SIZE ; ++i )
			{
#ifdef SSE_BOARD
				LineMask_128 oneRow = ifGuessBlackBoard.board.rowString[i] | ifGuessWhiteBoard.board.rowString[i] ;
#else
				LineMask oneRow = ifGuessBlackBoard.board.rowString[i] | ifGuessWhiteBoard.board.rowString[i] ;
#endif
				if ( oneRow != correctBoard.rowString[i] )
				{
					isUpdate = true ;
#ifdef SSE_BOARD
					correctBoard.numOfPaintedSquares += count128( ( correctBoard.rowString[i] ^ oneRow ) ) ;
#else
					correctBoard.numOfPaintedSquares += count64( ( correctBoard.rowString[i] ^ oneRow ) ) ;
					// cerr << "count64:" << count64( ( correctBoard.rowString[i] ^ oneRow ) ) << endl;
#endif
					correctBoard.rowString[i] = correctBoard.rowString[i] & oneRow ;
					// correctBoard.rowString[i] = correctBoard.rowString[i] | ( correctBoard.rowString[i] ^ oneRow ) ;
					NodeQueue::myQ.pushQ(i + P_SIZE) ;
				}

#ifdef SSE_BOARD
				LineMask_128 oneCol = ifGuessBlackBoard.board.colString[i] | ifGuessWhiteBoard.board.colString[i] ;
#else
				LineMask oneCol = ifGuessBlackBoard.board.colString[i] | ifGuessWhiteBoard.board.colString[i] ;
#endif
				if ( oneCol != correctBoard.colString[i] )
				{
					isUpdate = true ;
					correctBoard.colString[i] = correctBoard.colString[i] & oneCol;
					// correctBoard.colString[i] = correctBoard.colString[i] | ( correctBoard.colString[i] ^ oneCol );
					NodeQueue::myQ.pushQ(i);
				}
			} // for each line

			if ( isUpdate )
			{

				int state = lineSolver( problem, correctBoard ) ;

				if ( state == SOLVED )
					return SOLVED ;
				else if ( state == CONFLICT )
					return CONFLICT ;
				else CHANGE;
			}
// #endif
      return UNSOLVED ;
    } // else is make pair
  } // else is guess black 

  return UNSOLVED ;
}

inline void sendInverseLinksBlack( PresearchBoard &ifGuessBlackBoard, int pX, int pY )
{
	// black
	for ( int i = 0 ; i < ifGuessBlackBoard.board.updateArea.num ; i++ )
	{
		int x_ = ifGuessBlackBoard.board.updateArea.points[i].x ;
		int y_ = ifGuessBlackBoard.board.updateArea.points[i].y ;
		if ( pX == x_ && pY == y_ )
			continue ;

		int squareType = ifGuessBlackBoard.board.updateArea.points[i].squareType ;


		// do inversing here..  
		if ( squareType == SQUARE_SPACE )
		{
			if (!checkWhetherInList(ifGuessWhiteBoards[x_][y_], pX, pY, SQUARE_BLOCK)) // 反向填白
			{
				ifGuessWhiteBoards[x_][y_].updateList.pushQ(pX, pY, SQUARE_BLOCK);
#ifdef SENDQUEUE_IN_INVERSE_LINKS
				NodeQueue::pQ.pushQ(x_, y_);
#endif
			}
		}
		else
		{
			if (!checkWhetherInList(ifGuessBlackBoards[x_][y_], pX, pY, SQUARE_BLOCK))
			{
				ifGuessBlackBoards[x_][y_].updateList.pushQ(pX, pY, SQUARE_BLOCK);
#ifdef SENDQUEUE_IN_INVERSE_LINKS
				NodeQueue::pQ.pushQ(x_, y_);
#endif
			}
		}
	}

	ifGuessBlackBoard.updateList.listUpdated  = false;
	return;
}

inline void sendInverseLinksWhite( PresearchBoard &ifGuessWhiteBoard, int pX, int pY )
{
	// white
	for ( int i = 0 ; i < ifGuessWhiteBoard.board.updateArea.num ; ++i )
	{
		int x_ = ifGuessWhiteBoard.board.updateArea.points[i].x ;
		int y_ = ifGuessWhiteBoard.board.updateArea.points[i].y ;
		if ( pX == x_ && pY == y_ )
			continue ;

		int squareType = ifGuessWhiteBoard.board.updateArea.points[i].squareType ;

		// do inversing here..        
		if ( squareType == SQUARE_SPACE)
		{
			if (!checkWhetherInList(ifGuessWhiteBoards[x_][y_], pX, pY, SQUARE_SPACE))
			{
				ifGuessWhiteBoards[x_][y_].updateList.pushQ(pX, pY, SQUARE_SPACE);
#ifdef SENDQUEUE_IN_INVERSE_LINKS
				NodeQueue::pQ.pushQ(x_, y_);
#endif
			}
		}
		else
		{
			if (!checkWhetherInList(ifGuessBlackBoards[x_][y_], pX, pY, SQUARE_SPACE))
			{
				ifGuessBlackBoards[x_][y_].updateList.pushQ(pX, pY, SQUARE_SPACE);
#ifdef SENDQUEUE_IN_INVERSE_LINKS
				NodeQueue::pQ.pushQ(x_, y_);
#endif
			}
		}
	}

	ifGuessWhiteBoard.updateList.listUpdated = false;
	return;
}

inline bool checkWhetherInList(PresearchBoard &pPresearchBoard, int pX, int pY, int pType)
{
	if ( getSquare( pX, pY, pPresearchBoard.board )==pType ) return true;

	return checkWhetherInList2(pPresearchBoard.updateList, pX, pY, pType);
}

inline bool checkWhetherInList2(UpdateList& updateList, int pX, int pY, int pType)
{
	return updateList.isInQ(pX, pY, pType);
}

inline bool checkWhetherBoardsAreSame(Board &b1, Board &b2)
{

	for (int i=0; i<P_SIZE; ++i)
	{
#ifdef SSE_BOARD
		LineMask_128 oneCol = b1.colString[i] | b2.colString[i] ;
#else
		LineMask oneCol = b1.colString[i] | b2.colString[i] ;
#endif
		if ( oneCol != b1.colString[i] )
		{
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////
// Fully Probing (PreSearch), 2SAT here!!
extern unsigned char DPtable[P_SIZE+1][(P_SIZE+1)/2+1];
ProbingQueue NodeQueue::pQ;

inline int beginFullyProbing( const Puzzle& problem, 
    Board& correctBoard, 
    SquareToGo& whereCanIgo,
    int nodeCount, 
	int pLevel,
	Board& solution2
    )
{
	// main
	whereCanIgo.firstGo.numOfPaintedSquares = -1;
	whereCanIgo.secondGo.numOfPaintedSquares = -1;
#ifdef TWO_SAT
if ( nodeCount == 1 ) {
  char tmp[100] ;
  sprintf( tmp, "TWO_SAT/$%d.gv", currentId ) ;
  freopen( tmp, "a", stdout ) ;
  printf("digraph G {\n") ;
  bool subGraph[P_SIZE][P_SIZE] ;
  memset( subGraph, false, sizeof( subGraph ) ) ;
  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
  for ( int j = 0 ; j < P_SIZE ; j++ )
    {
    if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN ) 
      {
      for ( int k = j + 1 ; k < P_SIZE ; k++ )
        {
        if ( getSquare( i, k, correctBoard ) == SQUARE_UNKNOWN ) 
          {
          // b b
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          LineMask settle;
          LineMask line = correctBoard.colString[i] ;

		  // void *qLine = (void *) &line;
		  setLine(j, k, (void *) &line, SQUARE_SPACE, SQUARE_SPACE);

          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ j ];
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];

          LineNumbers lineNumber = problem.mLines[i] ;
          if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[k][i] = true ;
            printf("\"(%d, %d)\"->\"-(%d, %d)\";\n", j, i, k, i) ;
            }

          // b w
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.colString[i] ;
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ j ];
          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];

		  setLine(j, k, &line, SQUARE_SPACE, SQUARE_BLOCK);
          lineNumber = problem.mLines[i] ;
          if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[k][i] = true ;
            printf("\"(%d, %d)\"->\"(%d, %d)\";\n", j, i, k, i) ;
            }

          // w b
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.colString[i] ;
          
		  // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ j ];
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
		  setLine(j, k, &line, SQUARE_BLOCK, SQUARE_SPACE);
          lineNumber = problem.mLines[i] ;
          if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[k][i] = true ;
            printf("\"-(%d, %d)\"->\"-(%d, %d)\";\n", j, i, k, i) ;
            }

          // w w
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.colString[i] ;
          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ j ];
          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
		  setLine(j, k, &line, SQUARE_BLOCK, SQUARE_BLOCK);
          lineNumber = problem.mLines[i] ;
          if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[k][i] = true ;
            printf("\"-(%d, %d)\"->\"(%d, %d)\";\n", j, i, k, i) ;
            }
          } 
        }

      for ( int k = i + 1 ; k < P_SIZE ; k++ )
        {
        if ( getSquare( k, j, correctBoard ) == SQUARE_UNKNOWN ) 
          {
          // b b
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          LineMask settle;
          LineMask line = correctBoard.rowString[j] ;

          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ i ];
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
		  setLine(i, k, &line, SQUARE_SPACE, SQUARE_SPACE);
          LineNumbers lineNumber = problem.mLines[j + P_SIZE] ;
          if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[j][k] = true ;
            printf("\"(%d, %d)\"->\"-(%d, %d)\";\n", j, i, j, k) ;
            }

          // b w
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.rowString[j] ;
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ i ];
          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
		  setLine(i, k, &line, SQUARE_SPACE, SQUARE_BLOCK);
          lineNumber = problem.mLines[j + P_SIZE] ;
          if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[j][k] = true ;
            printf("\"(%d, %d)\"->\"(%d, %d)\";\n", j, i, j, k) ;
            }

          // w b
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.rowString[j] ;

          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ i ];
          // line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
		  setLine(i, k, &line, SQUARE_BLOCK, SQUARE_SPACE);
          lineNumber = problem.mLines[j + P_SIZE] ;
          if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[j][k] = true ;
            printf("\"-(%d, %d)\"->\"-(%d, %d)\";\n", j, i, j, k) ;
            }

          // w w
          memset(DPtable,UNSOLVED,sizeof(DPtable));
          line = correctBoard.rowString[j] ;

          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ i ];
          // line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
		  setLine(i, k, &line, SQUARE_BLOCK, SQUARE_BLOCK); 
          lineNumber = problem.mLines[j + P_SIZE] ;
          if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
            subGraph[j][i] = true ;
            subGraph[j][k] = true ;
            printf("\"-(%d, %d)\"->\"(%d, %d)\";\n", j, i, j, k) ;
            }
          } 
        }
      }
    }
  }

  for ( int i = 0 ; i < P_SIZE ; i++ )
    for ( int j = 0 ; j < P_SIZE ; j++ )
      if ( subGraph[i][j] )
      {
        printf("subgraph cluster%d_%d {\n", i, j ) ;
        printf("\"(%d, %d)\";\n\"-(%d, %d)\";\n}\n", i, j, i, j) ;
      }

  printf("}\n") ;
  }
#endif

    Board resumeBoard ;
	Board origBoard = correctBoard;

	int lasti = -1;
	int lastj = -1;

#ifdef DEBUG_TREENODES
	cout << "Level:" << pLevel << endl;
	printPuzzle(problem, correctBoard);
#endif

	// probing queue initial..
	int qInitCount = 0;
	NodeQueue::pQ.initial();
	for ( int i = 0 ; i < P_SIZE; ++i )
		for ( int j = 0 ; j < P_SIZE ; ++j )
		{
			if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN )
			{
				NodeQueue::pQ.pushQ(i,j);

				// 2012.07.16 由於會 copy probing nodes 下來,所以,每一個的 initial painted 個數會不一樣..
				// 這樣影響到 moving select 的基準, 可能造成變差(?)
				numberPaintedInBlackBoards[i][j] = ifGuessBlackBoards[i][j].board.numOfPaintedSquares;
				numberPaintedInWhiteBoards[i][j] = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares;
				++qInitCount;
			}
		}

	
	int qRunCount = 0;
    while ( true )
    {
		memcpy(resumeBoard.colString, correctBoard.colString, sizeof(correctBoard.colString)) ;
		memcpy(resumeBoard.rowString, correctBoard.rowString, sizeof(correctBoard.rowString)) ;

		// resumeBoard = correctBoard
#ifdef DEBUG_TREENODES
		cout <<  "UPDATE in Probing.." << endl;
		printPuzzle(problem, correctBoard);
#endif

#if MODE==3
		bool qFoundSolved = false;
#endif
		while (!NodeQueue::pQ.isEmpty())
		{
			++qRunCount;
			OneCell picked = NodeQueue::pQ.popAndFront();
			int qX = picked.x;
			int qY = picked.y;

			// For One Probing..
			if ( getSquare( qX, qY, correctBoard ) != SQUARE_UNKNOWN ) continue; 		

			NodeQueue::myQ.totalProbing += 1;
			int state = beginOneProbe( problem,
				correctBoard, 
				ifGuessBlackBoards[qX][qY],
				ifGuessWhiteBoards[qX][qY],
				qX,
				qY // ,
				// (qRunCount<=qInitCount) // 是否第一次跑這個點?
				) ;
			// fflush( stdout ) ;

			if ( state == SOLVED )
			{
				// 2012.08.01 雖然一邊有解了,另一邊有可能有另一解!
				/*whereCanIgo.firstGo =  ifGuessBlackBoards[qX][qY].board;
				whereCanIgo.secondGo = ifGuessWhiteBoards[qX][qY].board;
				whereCanIgo.firstGo.lastColor = SQUARE_SPACE;
				whereCanIgo.secondGo.lastColor = SQUARE_BLOCK;*/

#if MODE==3
#ifdef DEBUG_TREENODES
				cout << "qX:" << qX << " qY:" << qY << endl;
				cout << ifGuessBlackBoards[qX][qY].board.numOfPaintedSquares << "@" << ifGuessWhiteBoards[qX][qY].board.numOfPaintedSquares << endl;
#endif
				if (ifGuessBlackBoards[qX][qY].board.numOfPaintedSquares==S_SIZE 
					&& ifGuessWhiteBoards[qX][qY].board.numOfPaintedSquares == S_SIZE
					)
				{
					correctBoard = ifGuessBlackBoards[qX][qY].board;
					solution2    = ifGuessWhiteBoards[qX][qY].board;
					printPuzzle(problem, ifGuessBlackBoards[qX][qY].board);
					printPuzzle(problem, ifGuessWhiteBoards[qX][qY].board);
					return SOLVED;
				}
				else if (ifGuessBlackBoards[qX][qY].board.numOfPaintedSquares==S_SIZE)
				{
					printPuzzle(problem, ifGuessBlackBoards[qX][qY].board);
					if (solution2.numOfPaintedSquares!=S_SIZE) solution2 = ifGuessBlackBoards[qX][qY].board;

					memcpy(correctBoard.colString, resumeBoard.colString, sizeof(correctBoard.colString));
					memcpy(correctBoard.rowString, resumeBoard.rowString, sizeof(correctBoard.rowString));
					mergeBoard(correctBoard, ifGuessWhiteBoards[qX][qY].board, true);
					// correctBoard = ifGuessWhiteBoards[qX][qY].board;
					qFoundSolved = true;
					// NodeQueue::pQ.clear();
					NodeQueue::pQ.pushQ(qX, qY);
					break;
				} else if (ifGuessWhiteBoards[qX][qY].board.numOfPaintedSquares==S_SIZE)
				{
					printPuzzle(problem, ifGuessWhiteBoards[qX][qY].board);
					if (solution2.numOfPaintedSquares!=S_SIZE) solution2 = ifGuessWhiteBoards[qX][qY].board;

					memcpy(correctBoard.colString, resumeBoard.colString, sizeof(correctBoard.colString));
					memcpy(correctBoard.rowString, resumeBoard.rowString, sizeof(correctBoard.rowString));
					mergeBoard(correctBoard, ifGuessBlackBoards[qX][qY].board, true);
					// correctBoard = ifGuessBlackBoards[qX][qY].board;
					qFoundSolved = true;
					// NodeQueue::pQ.clear();
					NodeQueue::pQ.pushQ(qX, qY);
					break;
				}
#else
				return SOLVED;
#endif
			}
			else if ( state == CONFLICT )
			{
				return CONFLICT ;
			} 
			else if ( state==CHANGE )
			{
				lasti = qX;
				lastj = qY;
			}

		}
		

        if (  
#if MODE==3
			// 因為 else 的 job queue needs re-initialed.
			!qFoundSolved &&
#endif
			memcmp( resumeBoard.rowString, correctBoard.rowString, sizeof(correctBoard.rowString) ) ==0 ) 
// #endif
        {
			lasti = lastj = -1;

			// Move Ordering..
			int x = -1  ;
			int y = -1 ;
#ifdef ENABLE_BACKJUMPING
			if (NodeQueue::myQ.jumpSelectX!=-1 && NodeQueue::myQ.jumpSelectY!=-1)
			{
				x = NodeQueue::myQ.jumpSelectX;
				y = NodeQueue::myQ.jumpSelectY;

				NodeQueue::myQ.lastX = x;
				NodeQueue::myQ.lastY = y;
				if (NodeQueue::myQ.jumpSelectColor==SQUARE_BLOCK)
				{
					whereCanIgo.firstGo = ifGuessWhiteBoards[x][y].board;
					whereCanIgo.firstGo.lastColor = SQUARE_BLOCK;
					whereCanIgo.secondGo = ifGuessBlackBoards[x][y].board;
					whereCanIgo.secondGo.lastColor = SQUARE_SPACE;
				} else {
					whereCanIgo.secondGo = ifGuessWhiteBoards[x][y].board;
					whereCanIgo.secondGo.lastColor = SQUARE_BLOCK;
					whereCanIgo.firstGo = ifGuessBlackBoards[x][y].board;
					whereCanIgo.firstGo.lastColor = SQUARE_SPACE;
				}
				NodeQueue::myQ.jumpSelectX = NodeQueue::myQ.jumpSelectY = -1;
			} else {
#endif
			selectMove(problem, correctBoard, correctBoard, x, y) ;

			NodeQueue::myQ.lastX = x;
			NodeQueue::myQ.lastY = y;

			if (x!=-1 && y!=-1)
			{
				if ((ifGuessWhiteBoards[x][y].board.numOfPaintedSquares-correctBoard.numOfPaintedSquares)
	>(ifGuessBlackBoards[x][y].board.numOfPaintedSquares-correctBoard.numOfPaintedSquares))
				{
					whereCanIgo.firstGo = ifGuessWhiteBoards[x][y].board;
					whereCanIgo.firstGo.lastColor = SQUARE_BLOCK;
					whereCanIgo.secondGo = ifGuessBlackBoards[x][y].board;
					whereCanIgo.secondGo.lastColor = SQUARE_SPACE;
				} else {
					whereCanIgo.secondGo = ifGuessWhiteBoards[x][y].board;
					whereCanIgo.secondGo.lastColor = SQUARE_BLOCK;
					whereCanIgo.firstGo = ifGuessBlackBoards[x][y].board;
					whereCanIgo.firstGo.lastColor = SQUARE_SPACE;
				}
			} else {
				whereCanIgo.firstGo.numOfPaintedSquares = -1;
				whereCanIgo.secondGo.numOfPaintedSquares = -1;
			}

#ifdef ENABLE_BACKJUMPING
			}
#endif

			return UNSOLVED ;


        } // end if 2sat is stall
		else 
		{
			// 盤面若有變更者
// #ifdef DOING_INVERSE_LINKS
			NodeQueue::pQ.initial();
			for ( int i = 0 ; i < P_SIZE; ++i )
				for ( int j = 0 ; j < P_SIZE ; ++j )
				{
					if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN )
						NodeQueue::pQ.pushQ(i, j);
				}  
// #endif		
		} 
        
    } // end while


    return UNSOLVED ;
}


///////////////////////////////////////////////////
// Begin Solving..
int searchOneSolution( const Puzzle& problem,
    Board& solution1,
	Board& solution2,
    int& nodeCount
    ) 
{
	NodeQueue::myQ.totalQuery = 1;
	NodeQueue::myQ.totalHit = 0;
	NodeQueue::myQ.lastQuery = 1;
	NodeQueue::myQ.lastHit = 0;
	NodeQueue::myQ.totalProbing = 0;
	NodeQueue::myQ.totalMerging = 0;
	NodeQueue::myQ.totalPaintedByLineSolver = 0;

	NodeQueue::myQ.layerNodeCount = 0;
	NodeQueue::myQ.layerAverage = 0.0f;

#ifdef ENABLE_BACKJUMPING
	memset(NodeQueue::myQ.jumpLevels, 0, sizeof(unsigned short)*P_SIZE*P_SIZE);
#endif
	NodeQueue::myQ.lastX = NodeQueue::myQ.lastY = -1;


   // memset(dbTable.pointTables,0,sizeof(dbTable.pointTables));
   // dbTable.updateTable = 0 ;

   int state;
#ifdef TWO_SAT || DT
   state = searchBy2SatOrDT( problem, solution1 ) ;
#else
   state = lineSolver( problem, solution1 ) ;
#endif

   currentId++ ;
   // char tmp[100] ;sear
#ifdef TWO_SAT
   char tmp[100] ;
  sprintf( tmp, "TWO_SAT/$%d.gv", currentId ) ;
  freopen( tmp, "a", stdout ) ;
  printf("/*\n") ;
	printPuzzle( problem, solution1 ) ;
  printf("*/\n") ;
  fflush( stdout ) ;
#endif  
/*#ifdef DT
  sprintf( tmp, "DT/$%d.gv", currentId ) ;
  freopen( tmp, "w", stdout ) ;
  printf( "\n" ) ;
  printPuzzle( problem, solution1 ) ;
  printf("\n" ) ;
  UglyDt( problem, solution1 );
  fflush( stdout ) ;
#endif*/

  if (gLimitedLevel==1)
  {
	  NodeQueue::myQ.layerAverage = solution1.numOfPaintedSquares;
	  NodeQueue::myQ.layerNodeCount = 1;
	  return UNSOLVED;
  }

	initialPresearch( solution1 );
    if ( state == SOLVED )
        return SOLVED ;
    else if ( state == CONFLICT)
        return CONFLICT ;
    else
	{
        state = searchSolverDfs_Wrapper(problem, solution1, solution2, nodeCount, 1);//Solve2Sat(problem, solution) ;
		if (solution1.numOfPaintedSquares==S_SIZE 
			&& solution2.numOfPaintedSquares==S_SIZE
			)
		{
			if (checkWhetherBoardsAreSame(solution1,solution2)) solution2.numOfPaintedSquares = 0; 

		}
		return state;
	}
}

///////////////////////////////////////////////////
// Tree Search
extern int gLimitedLevel ;

#ifdef ENABLE_BACKJUMPING
void updateJumpLevelTable(Board &targetBoard, int level, bool pClean)
{
	for ( int i = 0 ; i<P_SIZE ; ++i )
		for ( int j = 0 ; j<P_SIZE ; ++j )
		{
			if ( getSquare( i, j, targetBoard ) != SQUARE_UNKNOWN )
			{
				if ( NodeQueue::myQ.jumpLevels[i][j]==0 || NodeQueue::myQ.jumpLevels[i][j]>level)
				{
					NodeQueue::myQ.jumpLevels[i][j] = level;
				}
			} 
			else if (pClean)
			{
				NodeQueue::myQ.jumpLevels[i][j]=0;
			}
		}
		
	return;
}

void updateJumpLevelTableWithAnd(Board &targetBoard, int level)
{
	for ( int i = 0 ; i<P_SIZE ; ++i )
		for ( int j = 0 ; j<P_SIZE ; ++j )
		{
			if ( getSquare( i, j, targetBoard ) != SQUARE_UNKNOWN )
			{
				if ( NodeQueue::myQ.jumpLevels[i][j]==level )
				{
					NodeQueue::myQ.jumpLevels[i][j] = level;
				}
			} 
			else 
			{
				NodeQueue::myQ.jumpLevels[i][j]=0;
			}
		}

		return;
}

void cleanJumpLevelTable(Board &targetBoard, int level)
{
	for (int i=0; i<P_SIZE; ++i)
		for (int j=0; j<P_SIZE; ++j)
		{
			if (NodeQueue::myQ.jumpLevels[i][j]>=level) NodeQueue::myQ.jumpLevels[i][j] = 0;
			else if ( getSquare( i, j, targetBoard ) == SQUARE_UNKNOWN ) NodeQueue::myQ.jumpLevels[i][j] = 0;
		}

}
#endif

int searchSolverDfs_Wrapper( const Puzzle& problem,
	Board& solution1, 
	Board& solution2, 
	int& nodeCount,
	int level
	)
{
#ifdef ENABLE_BACKJUMPING
	NodeQueue::myQ.targetJump = 9999999;
#endif

	// Board resumeBoard = solution1;
	int qRet = searchSolverDfs_Main(problem, solution1, solution2, nodeCount, level);

#ifdef ENABLE_BACKJUMPING
	cleanJumpLevelTable(solution1, level);
#endif

	return qRet;
}

int searchSolverDfs_Main( const Puzzle& problem,
    Board& solution1, 
	Board& solution2, 
    int& nodeCount,
    int level
    )
{
   if ( level == gLimitedLevel )
   {
      return UNSOLVED ;
   }

    SquareToGo whereCanIgo ;
	whereCanIgo.firstGo.numOfPaintedSquares = -1;
	whereCanIgo.secondGo.numOfPaintedSquares = -1;


#ifdef ENABLE_BACKJUMPING
	NodeQueue::myQ.targetJump = 9999999;
	int qLastParentX = NodeQueue::myQ.lastX;
	int qLastParentY = NodeQueue::myQ.lastY;
	int qLastParentColor = NodeQueue::myQ.lastColor;
#endif

    nodeCount++;
    int state = beginFullyProbing( problem, solution1,  whereCanIgo, nodeCount, level, solution2 ) ;

	// 201208. for layer painted counting..
	if (level == gLimitedLevel-1)
	{
		double qAverage = solution1.numOfPaintedSquares
			+ NodeQueue::myQ.layerAverage * NodeQueue::myQ.layerNodeCount;
		NodeQueue::myQ.layerAverage = qAverage/(1.0+NodeQueue::myQ.layerNodeCount);
		NodeQueue::myQ.layerNodeCount += 1;
	}

    if ( state == SOLVED )
	{
#if MODE==2 || MODE==3
		// 這裡判斷是否第二解,已存在?
		if (solution2.numOfPaintedSquares!=S_SIZE)
		{
			solution2 = solution1;
			// return UNSOLVED;
		} 
		else  if (checkWhetherBoardsAreSame(solution1, solution2)==false)
		{
#if MODE==2
			// 找到兩解了
			return SOLVED;
#else
			// MODE3, 全部印出來...
			// PS. 有可能兩個都是解
			if (whereCanIgo.firstGo.numOfPaintedSquares==S_SIZE)
				printPuzzle(problem, whereCanIgo.firstGo);
			if (whereCanIgo.secondGo.numOfPaintedSquares==S_SIZE)
				printPuzzle(problem, whereCanIgo.secondGo);
#endif
		} 

		// MODE2,3. UNSOLVED 的部份, 往另一個 node 去求解
		// cout << whereCanIgo.firstGo.numOfPaintedSquares << "@" << whereCanIgo.secondGo.numOfPaintedSquares << endl;

		/* if (whereCanIgo.firstGo.numOfPaintedSquares<S_SIZE
			&& whereCanIgo.firstGo.numOfPaintedSquares>0
			)
		{
			solution1 = whereCanIgo.firstGo;

		} else if ( whereCanIgo.secondGo.numOfPaintedSquares<S_SIZE
			&& whereCanIgo.secondGo.numOfPaintedSquares>0
			) 
		{
			solution1 = whereCanIgo.secondGo;
		} else return UNSOLVED; // 兩個都 0, or S_SIZE

		initialPresearch(solution1);
		return searchSolverDfs_Wrapper( problem, solution1, solution2, nodeCount, level+1 ) ; */

		return UNSOLVED;

#else
		// MODE 1.
        return SOLVED ;
#endif
	} 
	else if ( state == CONFLICT )
	{
        return CONFLICT ;
	}

	

#ifdef DEBUG_TREENODES
	cout << "num:" << whereCanIgo.firstGo.numOfPaintedSquares << "@" << whereCanIgo.secondGo.numOfPaintedSquares << endl;
#endif
	if (whereCanIgo.firstGo.numOfPaintedSquares<0 || whereCanIgo.secondGo.numOfPaintedSquares<0)
		return UNSOLVED;

	Board firstBoard = whereCanIgo.firstGo;
  
#ifdef ENABLE_BACKJUMPING
	NodeQueue::myQ.targetJump = 9999999;
#endif

#ifdef DEBUG_TREENODES
	printf("lv:%d (first, before guess)\n", level);
	printPuzzle(problem, solution1);
	printf("lv:%d (first)\n", level);
	NodeQueue::myQ.debug();
	printPuzzle(problem, firstBoard);

	/* printf("(0,20) b:%d w:%d\n", ifGuessBlackBoards[0][20].board.numOfPaintedSquares-solution1.numOfPaintedSquares, ifGuessWhiteBoards[0][20].board.numOfPaintedSquares-solution1.numOfPaintedSquares);
	printPuzzle(problem, ifGuessBlackBoards[0][20].board);
	printf("\n");
	printPuzzle(problem, ifGuessWhiteBoards[0][20].board); 

	printf("(0,21) b:%d w:%d\n", ifGuessBlackBoards[0][21].board.numOfPaintedSquares-solution1.numOfPaintedSquares, ifGuessWhiteBoards[0][21].board.numOfPaintedSquares-solution1.numOfPaintedSquares);
	printPuzzle(problem, ifGuessBlackBoards[0][21].board);
	printf("\n");
	printPuzzle(problem, ifGuessWhiteBoards[0][21].board);  */
#endif

	int qLastX = NodeQueue::myQ.lastX;
	int qLastY = NodeQueue::myQ.lastY;
	NodeQueue::myQ.lastColor = firstBoard.lastColor;
	int firstState;
	do 
	{
		// initialPresearch( firstBoard );
		// 2012.07.12 copy the probing nodes.
#ifdef COPY_PROBING_NODES
		pushProbingNodes(level, solution1, firstBoard);
#else
		initialPresearch( firstBoard );
#endif

#ifdef ENABLE_BACKJUMPING
		updateJumpLevelTable(solution1, level, true);
#endif
		firstState = searchSolverDfs_Wrapper( problem, firstBoard, solution2, nodeCount, level+1 ) ;
		if ( firstState == SOLVED )
		{   
#if MODE==2 || MODE==3
			// 這裡判斷是否第二解,已存在?
			if (solution2.numOfPaintedSquares!=S_SIZE)
			{
				solution2 = firstBoard;
			} 
			else if (checkWhetherBoardsAreSame(firstBoard, solution2)==false)
			{
#if MODE==2
				solution1 = firstBoard;
				return SOLVED;
#else
				// MODE3, 全部印出來...
				printPuzzle(problem, firstBoard);
				solution1.numOfPaintedSquares = 0;
				return UNSOLVED;
#endif
			}
#else
			solution1 = firstBoard ;
			return SOLVED ;
#endif
		} 

#ifdef ENABLE_BACKJUMPING
		if ( level > NodeQueue::myQ.targetJump ) 
		{
			return UNSOLVED;
		} 
		else if ( level==NodeQueue::myQ.targetJump
			&& getSquare(NodeQueue::myQ.lastX, NodeQueue::myQ.lastY, whereCanIgo.firstGo)==SQUARE_UNKNOWN
			) 
		{
			// setAndFlag(NodeQueue::myQ.lastX, NodeQueue::myQ.lastY, whereCanIgo.firstGo, (NodeQueue::myQ.lastColor==SQUARE_BLOCK)?SQUARE_SPACE:SQUARE_BLOCK);
			NodeQueue::myQ.lastX = qLastX;
			NodeQueue::myQ.lastY = qLastY; 
			firstBoard = whereCanIgo.firstGo;
			NodeQueue::myQ.lastColor = firstBoard.lastColor;
			initialPresearch( firstBoard );
			// return searchSolverDfs_Wrapper( problem, firstBoard, solution2, nodeCount, level);
		} else break;
#else 
		break;
#endif
	} while (1);
	

/* #ifdef DEBUG_TREENODES
	printf("lv:%d (first done)", level);
	printPuzzle(problem, firstBoard);
#endif*/
	
	Board secondBoard = whereCanIgo.secondGo;
#ifdef DEBUG_TREENODES
	printf("lv:%d (second) x:%d y:%d\n", level, qLastX, qLastY);
	NodeQueue::myQ.debug();
	printPuzzle(problem, secondBoard);
#endif
	
#ifdef ENABLE_BACKJUMPING
	NodeQueue::myQ.targetJump = 9999999;
#endif
	NodeQueue::myQ.lastX = qLastX;
	NodeQueue::myQ.lastY = qLastY;
	NodeQueue::myQ.lastColor = secondBoard.lastColor;
	int secondState;
	do 
	{
#ifdef ENABLE_BACKJUMPING
		updateJumpLevelTable(solution1, level, true);
#endif
		// initialPresearch( secondBoard );
		// 2012.07.12 restore the probing nodes.
#ifdef COPY_PROBING_NODES
		popProbingNodes(level, secondBoard);
#else
		initialPresearch( secondBoard );
#endif

		secondState = searchSolverDfs_Wrapper( problem, secondBoard, solution2, nodeCount, level+1 ) ;
		if ( secondState == SOLVED )
		{
#if MODE==2 || MODE==3
			// 這裡判斷是否第二解,已存在?
			if (solution2.numOfPaintedSquares!=S_SIZE)
			{
				solution2 = secondBoard;
			} 
			else if (checkWhetherBoardsAreSame(secondBoard, solution2)==false)
			{
#if MODE==2
				solution1 = secondBoard;
				return SOLVED;
#else
				// MODE3, 全部印出來...
				printPuzzle(problem, secondBoard);
				solution1.numOfPaintedSquares = 0;
				return UNSOLVED;
#endif
			}
#else
			solution1 = secondBoard ;
			return SOLVED ;
#endif
		}


#ifdef ENABLE_BACKJUMPING
		if (level > NodeQueue::myQ.targetJump ) 
		{
			return UNSOLVED;
		}  
		else if ( level==NodeQueue::myQ.targetJump
			&& getSquare(NodeQueue::myQ.lastX, NodeQueue::myQ.lastY, whereCanIgo.secondGo)==SQUARE_UNKNOWN
			) 
		{
			// setAndFlag(NodeQueue::myQ.lastX, NodeQueue::myQ.lastY, whereCanIgo.secondGo, (NodeQueue::myQ.lastColor==SQUARE_BLOCK)?SQUARE_SPACE:SQUARE_BLOCK);
			NodeQueue::myQ.lastX = qLastX;
			NodeQueue::myQ.lastY = qLastY;
			secondBoard = whereCanIgo.secondGo;
			NodeQueue::myQ.lastColor = secondBoard.lastColor;
			// initialPresearch( secondBoard );
			// return searchSolverDfs_Wrapper( problem, secondBoard, solution2, nodeCount, level);
		} else break;
#else
		break;
#endif
	} while (1);



#ifdef ENABLE_BACKJUMPING
	// 2012.05.24 only dead-end can do backjumping safely!!
	if ( firstState==CONFLICT && secondState==CONFLICT
		&& (NodeQueue::myQ.targetJump==9999999 && qLastX>=0 && qLastY>=0)
		/*&& (!checkWhetherBoardsAreSame(origBoard, solution1))*/ // 盤面要有變化!
		)
	{
		int maxLevel=0;
		for (int i=0; i<P_SIZE && maxLevel<level-1; ++i)
		{
			unsigned short tmp = NodeQueue::myQ.jumpLevels[i][qLastY]; 
			if (i!=qLastX  && tmp>0 && tmp>maxLevel) maxLevel=tmp;

			tmp = NodeQueue::myQ.jumpLevels[qLastX][i];
			if (i!=qLastY  && tmp>0 && tmp>maxLevel) maxLevel=tmp;

			// row..
			/* bool hasTarget = false;
			int maxLine = 0;
			for (int j=0; j<P_SIZE && maxLevel<level-1; ++j)
			{
				unsigned short tmp = NodeQueue::myQ.jumpLevels[i][j];
				if (tmp==level+1) hasTarget=true;
				else if (tmp>0 && tmp>maxLine) maxLine=tmp;
			}
			if (hasTarget && maxLine>maxLevel) maxLevel=maxLine;

			// column..
			hasTarget = false;
			maxLine = 0;
			for (int j=0; j<P_SIZE && maxLevel<level-1; ++j)
			{
				unsigned short tmp = NodeQueue::myQ.jumpLevels[j][i];
				if (tmp==level+1) hasTarget=true;
				else if (tmp>0 && tmp>maxLine) maxLine=tmp;
			}
			if (hasTarget && maxLine>maxLevel) maxLevel=maxLine;*/
		}

		if (maxLevel>0 && maxLevel<level-2)
		{
			// cerr << "Jump to level:" << maxLevel << " from:" << level << endl;
#ifdef ENABLE_BACKJUMPING_DEBUG
			if (maxLevel<level)
			{
				cout << "X:" << qLastX << " Y:" << qLastY << " V:" << NodeQueue::myQ.jumpLevels[qLastX][qLastY] << endl;
				cout << "Jump to level:" << maxLevel << " from:" << level << endl;
				cerr << "Jump to level:" << maxLevel << " from:" << level << endl;
				printPuzzle(problem, solution1);
				NodeQueue::myQ.debugJumpTable();
				printPuzzle(problem, firstBoard);
				cout << endl;
				printPuzzle(problem, secondBoard);
			}
#endif
			NodeQueue::myQ.targetJump = maxLevel;
			
			NodeQueue::myQ.jumpSelectX = qLastParentX;
			NodeQueue::myQ.jumpSelectY = qLastParentY;
			NodeQueue::myQ.jumpSelectColor = (qLastParentColor==SQUARE_BLOCK)?SQUARE_SPACE:SQUARE_BLOCK;
		} 
	} 
#endif
    return UNSOLVED ;
}

// return true, if CONFLICT..
#ifndef SSE_BOARD
void pushProbingNodes(int pLevel, Board &pBoard, Board &pFirstBoard)
{
  NodeQueue::myQ.initial() ;
  size_t pSize = sizeof(ifGuessWhiteBoards[0][0].board.colString);

  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    for ( int j = 0 ; j < P_SIZE ; j++ )
    {  
		  // if ( getSquare(i, j, pBoard)!=SQUARE_UNKNOWN ) continue;

		  // 以下這段減少 updateArea 的 copy <-- 會省很多!
		  memcpy(fullyProbingWhiteBoards[pLevel][i][j].colString, ifGuessWhiteBoards[i][j].board.colString, pSize);
		  memcpy(fullyProbingWhiteBoards[pLevel][i][j].rowString, ifGuessWhiteBoards[i][j].board.rowString, pSize);
		  memcpy(fullyProbingBlackBoards[pLevel][i][j].colString, ifGuessBlackBoards[i][j].board.colString, pSize);
		  memcpy(fullyProbingBlackBoards[pLevel][i][j].rowString, ifGuessBlackBoards[i][j].board.rowString, pSize);

		  fullyProbingWhiteBoards[pLevel][i][j].numOfPaintedSquares = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares;
		  fullyProbingBlackBoards[pLevel][i][j].numOfPaintedSquares = ifGuessBlackBoards[i][j].board.numOfPaintedSquares;

#ifdef DOING_INVERSE_LINKS
		  ifGuessWhiteBoards[i][j].updateList.clear();
		  ifGuessBlackBoards[i][j].updateList.clear();
#endif

		  ifGuessWhiteBoards[i][j].board.updateArea.num = 0;
		  ifGuessBlackBoards[i][j].board.updateArea.num = 0;

		  // if ( getSquare(i, j, pBoard)!=SQUARE_UNKNOWN ) continue;

		  /* memcpy(ifGuessWhiteBoards[i][j].board.colString, pFirstBoard.colString, pSize);
		  memcpy(ifGuessWhiteBoards[i][j].board.rowString, pFirstBoard.rowString, pSize);
		  memcpy(ifGuessBlackBoards[i][j].board.colString, pFirstBoard.colString, pSize);
		  memcpy(ifGuessBlackBoards[i][j].board.rowString, pFirstBoard.rowString, pSize);

		  ifGuessWhiteBoards[i][j].board.numOfPaintedSquares = pFirstBoard.numOfPaintedSquares;
		  ifGuessBlackBoards[i][j].board.numOfPaintedSquares = pFirstBoard.numOfPaintedSquares; */

		  // 以上是 FullyProbing 後的東西
		  // mergeBoard(ifGuessWhiteBoards[i][j].board, pFirstBoard, false);
		  // mergeBoard(ifGuessBlackBoards[i][j].board, pFirstBoard, false);  
	}
  }

  return;
}

// return true, if CONFLICT..
void popProbingNodes(int pLevel, Board &pBoard)
{
	NodeQueue::myQ.initial() ;
	size_t pSize = sizeof(ifGuessWhiteBoards[0][0].board.colString);

	for ( int i = 0 ; i < P_SIZE ; i++ )
	{
		for ( int j = 0 ; j < P_SIZE ; j++ )
		{  
			// if ( getSquare(i, j, pBoard)!=SQUARE_UNKNOWN ) continue;

			// 以下這段減少 updateArea 的 copy <-- 會省很多!
			memcpy(ifGuessWhiteBoards[i][j].board.colString, fullyProbingWhiteBoards[pLevel][i][j].colString,  pSize);
			memcpy(ifGuessWhiteBoards[i][j].board.rowString, fullyProbingWhiteBoards[pLevel][i][j].rowString, pSize);
			memcpy(ifGuessBlackBoards[i][j].board.colString, fullyProbingBlackBoards[pLevel][i][j].colString, pSize);
			memcpy(ifGuessBlackBoards[i][j].board.rowString, fullyProbingBlackBoards[pLevel][i][j].rowString, pSize);

			ifGuessWhiteBoards[i][j].board.numOfPaintedSquares = fullyProbingWhiteBoards[pLevel][i][j].numOfPaintedSquares;
			ifGuessBlackBoards[i][j].board.numOfPaintedSquares = fullyProbingBlackBoards[pLevel][i][j].numOfPaintedSquares;

			/* memcpy(ifGuessWhiteBoards[i][j].board.colString, pBoard.colString, pSize);
			memcpy(ifGuessWhiteBoards[i][j].board.rowString, pBoard.rowString, pSize);
			memcpy(ifGuessBlackBoards[i][j].board.colString, pBoard.colString, pSize);
			memcpy(ifGuessBlackBoards[i][j].board.rowString, pBoard.rowString, pSize);

			ifGuessWhiteBoards[i][j].board.numOfPaintedSquares = pBoard.numOfPaintedSquares;
			ifGuessBlackBoards[i][j].board.numOfPaintedSquares = pBoard.numOfPaintedSquares; */

#ifdef DOING_INVERSE_LINKS
			ifGuessWhiteBoards[i][j].updateList.clear();
			ifGuessBlackBoards[i][j].updateList.clear();
#endif

			ifGuessWhiteBoards[i][j].board.updateArea.num = 0;
			ifGuessBlackBoards[i][j].board.updateArea.num = 0;

			// if ( getSquare(i, j, pBoard)!=SQUARE_UNKNOWN ) continue;

			// 以上是 FullyProbing 後的東西
			// mergeBoard(ifGuessWhiteBoards[i][j].board, pBoard, false);
			// mergeBoard(ifGuessBlackBoards[i][j].board, pBoard, false);  
		}
	}

	return;
}
#endif

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

/*bool checkDt(const Puzzle problem, Board& solution) 
{
  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    for ( int j = 0 ; j < P_SIZE ; j++ )
    {
      if ( getSquare( i, j, solution ) == SQUARE_UNKNOWN ) 
      {
            if ( !IsLegal(problem, ifGuessBlackBoards[i][j].board) )
            {
              solution = ifGuessWhiteBoards[i][j].board ;
              FILE* ft = fopen("dt.txt", "a+") ;
              fprintf(ft, "$%d dt work!\n", currentId) ;
              fclose(ft) ;
              return true ; 
            }

            if ( !IsLegal(problem, ifGuessWhiteBoards[i][j].board) )
            {
              solution = ifGuessBlackBoards[i][j].board ;
              FILE* ft = fopen("dt.txt", "a+") ;
              fprintf(ft, "$%d dt work!\n", currentId) ;
              fclose(ft) ;
              return true ; 
            }
      }
    }
  }

  return false ;
}*/

// void selectMove( Puzzle problem, Board correctBoard, int& x, int& y) 
void selectMove( const Puzzle &problem, Board &correctBoard, Board &origBoard, int& x, int& y)
  {

#ifdef WEIGHTED  

  double maxScore = -1 ;
  double maxMinorScore = -1;
  double score_minor = -1;

  for ( int i = 0 ; i < P_SIZE ; i++ )
    {
    for ( int j = 0 ; j < P_SIZE ; j++ )
      {
      if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN ) 
        {
#ifdef ADD_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
        double score = diff_black + diff_white;
#endif
#ifdef MAX_MOVE
        // double score = max( ifGuessBlackBoard[i][j].numOfSquareOnBoard, ifGuessWhiteBoard[i][j].numOfSquareOnBoard ) ;
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		double score = max( diff_black, diff_white);
#endif

#ifdef MUL_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
        double score = diff_black * diff_white ;
#endif

#ifdef MIN_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
        double score = min( diff_black, diff_white );

		score_minor = max( ifGuessBlackBoards[i][j].board.numOfPaintedSquares, ifGuessWhiteBoards[i][j].board.numOfPaintedSquares ) ;

		// 2012.07.22 若變化只有猜點的部份,那結果和 Random 沒兩樣?
		// if (score-origBoard.numOfPaintedSquares<20) score = S_SIZE;
#endif


#ifdef SQRT_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		double score = min( diff_black, diff_white ) + sqrt( (double) max( diff_black, diff_white ) / (double) ( min( diff_black, diff_white ) + 1 ));

#endif

#ifdef LOG_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
        double score = min( diff_black, diff_white ) + log(diff_black + 1.0)*log(diff_white + 1.0);
#endif

#ifdef MIN_LOG_MOVE
		int diff_black = ifGuessBlackBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		int diff_white = ifGuessWhiteBoards[i][j].board.numOfPaintedSquares - origBoard.numOfPaintedSquares;
		double score = min( diff_black, diff_white ) + MOVE_LOG_NUMBER * log ( 1.0 + (double)abs( diff_black - diff_white ) );

		// cout << "i:" << i << " j:" << j << " s:" << score << " org:" << origBoard.numOfPaintedSquares << endl;

		

#ifdef DEBUG_TREENODES_2
		// if (i==0 && j==16) cout << "i:" << i << " j:" << j << " s:" << score << endl;
		// if (i==1 && j==16) cout << "i:" << i << " j:" << j << " s:" << score << endl;
		if (i==1 && j==16) 
		{
			cout << "!!!!!!!!!!!!!!!!!!!!!" << endl;
			cout << "i:" << i << " j:" << j << " b:" << (ifGuessBlackBoards[i][j].board.numOfPaintedSquares-correctBoard.numOfPaintedSquares) << " w:" << (ifGuessWhiteBoards[i][j].board.numOfPaintedSquares-correctBoard.numOfPaintedSquares) << endl;


			printPuzzle(problem, ifGuessBlackBoards[i][j].board);
			printf("\n");
			printPuzzle(problem, ifGuessWhiteBoards[i][j].board);
			printf("=========================\n");
		}
#endif
#endif

#ifndef RANDOM_MOVE 
          if ( score>=0.0 )
          {
			  if (score>maxScore)
			  {
				  x = i ;
				  y = j ;
				  maxScore = score ;
				  maxMinorScore = score_minor;
			  } 
			  else if (score==maxScore) 
			  {
				  // 2012.05.18 越中心越好.
				  /* if (abs(i+j-P_SIZE)>abs(x+y-P_SIZE))
				  {
					  x = i;
					  y = j;
				  }*/

				  // 2012.07.22 compare score_minor if score are same..
				  if (score_minor>maxMinorScore) 
				  {
					  maxMinorScore = score_minor;
					  x = i; y = j;
				  } 
				  else if (score_minor==maxMinorScore)
				  {
					  // 2012.05.18 越中心越好.
					  if (abs(i+j-P_SIZE)>abs(x+y-P_SIZE))
					  {
						  x = i;
						  y = j;
					  }

				  }

			  }
			 
          }
#endif
        }
      }
    }

#ifdef DEBUG_TREENODES
	cout << "x:" << x << " y:" << y << endl;
    cout << "max:" << maxScore << " b:" << (ifGuessBlackBoards[x][y].board.numOfPaintedSquares-correctBoard.numOfPaintedSquares) << " w:" << (ifGuessWhiteBoards[x][y].board.numOfPaintedSquares - correctBoard.numOfPaintedSquares) << endl;
#endif
#endif     

#ifdef RANDOM_MOVE            
  while (true)
    {
    x = rand() % P_SIZE ;
    y = rand() % P_SIZE ;

    if ( getSquare( x, y, correctBoard ) == SQUARE_UNKNOWN)
      break ;
    }
#endif

#ifdef LINE_WEIGHTED_MOVE      
  int rowCount[P_SIZE] ;
  int colCount[P_SIZE] ;
  memset( rowCount, 0, sizeof(rowCount) ) ;
  memset( colCount, 0, sizeof(colCount) ) ;

  for ( int i = 0 ; i < P_SIZE ; i++ )
    {
    for ( int j = 0 ; j < P_SIZE ; j++ )
      {
      if ( getSquare( i, j, correctBoard ) != SQUARE_UNKNOWN )
        {
        colCount[i]++ ;
        rowCount[j]++ ;
        }
      }
    }

  int maxScore = -1 ;
  for ( int i = 0 ; i < P_SIZE ; i++ )
    {
    for ( int j = 0 ; j < P_SIZE ; j++ )
      {
      if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN )
        {
        int score = colCount[i] + rowCount[j] ;
        if ( score > maxScore )
          {
          x = i ;
          y = j ;
          maxScore = score ;
          }
        }
      }
    }
#endif
  //dbTable.updateTable |= dbTable.pointTables[x][y];

  }

int searchBy2SatOrDT( const Puzzle& problem, Board& solution ) 
{
  int state = lineSolver( problem, solution ) ;
  if ( state == SOLVED )
    return SOLVED ;
  else if ( state == CONFLICT )
    return CONFLICT ;

  return Solve2SatOrDT( problem, solution ) ;
}

inline int updateInverseList(const Puzzle &problem, UpdateList& updateList, Board& setBoard, int pX, int pY) 
{   
  int state = UNSOLVED ;
#ifdef DOING_INVERSE_LINKS
  // if (updateList.listUpdated==false) return UNSOLVED;
  int newListCount = 0;
  for ( int i = 0 ; i < updateList.num ; i++ )
  {
    int x_ = updateList.points[i].x ;
    int y_ = updateList.points[i].y ;
    int squareType = updateList.points[i].squareType ;

#ifdef DOING_INVERSE_LINKS_BY_COPY_ONE
	// 只拷貝一點
	int qBoardType = getSquare( x_, y_, setBoard );
	if ( qBoardType == squareType )
	{
      NodeQueue::myQ.initial() ;
	  setBoard.updateArea.num = 0 ;
	  updateList.clear();

      return CONFLICT ;
    }
    else if ( qBoardType == SQUARE_UNKNOWN )
    {
	   setAndFlag( x_, y_, setBoard, squareType ) ;
	   NodeQueue::myQ.totalMerging += 1;
       // if (i>=updateList.lastnum) setAndFlag( x_, y_, setBoard, squareType ) ;
	   // else setButNoFlag( x_, y_, setBoard, squareType );
    }
#else
    // 整個盤面拷貝
	if ( getSquare( x_, y_, setBoard ) == squareType )
	{
		NodeQueue::myQ.initial() ;
		setBoard.updateArea.num = 0;
		updateList.clear();
		/*state = CONFLICT ;
		updateList.num = 0 ;
		updateList.lastnum = 0;
		updateList.listUpdated = false;*/
		// UpdateList.listUpdated = false;

		return CONFLICT ;
	}
	else if ( getSquare( x_, y_, setBoard ) == SQUARE_UNKNOWN )
	{
		// setAndFlag( x_, y_, setBoard, squareType ) ;
		/* if (i>=updateList.lastnum) setAndFlag( x_, y_, setBoard, squareType ) ;
		else setButNoFlag( x_, y_, setBoard, squareType ); */
		setAndFlag( x_, y_, setBoard, squareType ) ;
		NodeQueue::myQ.totalMerging += 1;

#ifdef DEBUG_TREENODES
		if (pX==1 && pY==16) cout << " u x:" <<  x_ <<  " y:" << y_ << " s:" << squareType << endl;
#endif
		Board *srcBoard =NULL;
		if ( squareType == SQUARE_SPACE )
		{
			srcBoard = &(ifGuessBlackBoards[x_][y_].board) ;
		} else {
			srcBoard = &(ifGuessWhiteBoards[x_][y_].board) ;
		}

		// 拷貝整個盤面...
		for (int j=0; j<srcBoard->updateArea.num; ++j)
		{
			int xx = srcBoard->updateArea.points[j].x ;
			int yy = srcBoard->updateArea.points[j].y ;
			int ss = srcBoard->updateArea.points[j].squareType ;
			if ( getSquare( xx, yy, setBoard ) == ss )
			{
				NodeQueue::myQ.initial() ;
				setBoard.updateArea.num = 0;
				updateList.clear();

				return CONFLICT ;
			}
			else if ( getSquare( xx, yy, setBoard ) == SQUARE_UNKNOWN )
			{
				// setAndFlag( xx, yy, setBoard, ss ) ;
				/* if (i<updateList.lastnum) setButNoFlag( xx, yy, setBoard, ss );
				else */ 
				{
					setAndFlag( xx, yy, setBoard, ss ) ;

					// 2012.06.17 若新的點,不在 updateList 的話,加入 updateList, 以加入相對應的盤面
					if (i>=updateList.lastnum && !checkWhetherInList2(updateList, xx, yy, ss))
					{
						updateList.pushQ(xx,yy,ss);
					}

					// srcboard 的 upateList 也拷貝..
					UpdateList *srcUpdateList = NULL;
					if ( ss == SQUARE_SPACE )
					{
						srcUpdateList = &(ifGuessBlackBoards[xx][yy].updateList) ;
					} else {
						srcUpdateList = &(ifGuessWhiteBoards[xx][yy].updateList) ;
					}
					for (int uuu=0; uuu<srcUpdateList->num; ++uuu)
					{
						int xx2 = srcUpdateList->points[uuu].x ;
						int yy2 = srcUpdateList->points[uuu].y ;
						int ss2 = srcUpdateList->points[uuu].squareType ;
						if (!checkWhetherInList2(updateList, xx2, yy2, ss2))
						{
							updateList.pushQ(xx2, yy2, ss2);
						}

					}

				}
			}
		}
	}
    
#endif
  }

  // 2012.05.16 這次 lineSolve 做過的,下次就不用做了
  // updateList.lastnum = updateList.num;
  /* updateList.num = 0;
  updateList.lastnum = 0;
  updateList.listUpdated = false; */

  updateList.clear();
#endif

  return lineSolver( problem, setBoard, false ) ;
}

inline void initialPresearch( Board &correctBoard )
{
  NodeQueue::myQ.initial() ;

  for ( int i = 0 ; i < P_SIZE ; i++ )
  { 
    for ( int j = 0 ; j < P_SIZE ; j++ )
    {
	  if ( getSquare( i, j, correctBoard ) == SQUARE_UNKNOWN )
	  {
#ifdef DOING_INVERSE_LINKS
		  ifGuessWhiteBoards[i][j].updateList.clear();
		  ifGuessBlackBoards[i][j].updateList.clear();

		  /*ifGuessWhiteBoards[i][j].updateList.num = 0 ;
		  ifGuessBlackBoards[i][j].updateList.num = 0 ;
		  ifGuessWhiteBoards[i][j].updateList.lastnum = 0 ;
		  ifGuessBlackBoards[i][j].updateList.lastnum = 0 ;
		  ifGuessWhiteBoards[i][j].updateList.listUpdated = false;
		  ifGuessBlackBoards[i][j].updateList.listUpdated = false; */
#endif
		  
		  // 以下這段減少 updateArea 的 copy <-- 會省很多!
		  memcpy(ifGuessWhiteBoards[i][j].board.colString, correctBoard.colString, sizeof(correctBoard.colString));
		  memcpy(ifGuessWhiteBoards[i][j].board.rowString, correctBoard.rowString, sizeof(correctBoard.rowString));
		  memcpy(ifGuessBlackBoards[i][j].board.colString, correctBoard.colString, sizeof(correctBoard.colString));
		  memcpy(ifGuessBlackBoards[i][j].board.rowString, correctBoard.rowString, sizeof(correctBoard.rowString));

		  ifGuessWhiteBoards[i][j].board.numOfPaintedSquares = correctBoard.numOfPaintedSquares;
		  ifGuessBlackBoards[i][j].board.numOfPaintedSquares = correctBoard.numOfPaintedSquares;

		  ifGuessWhiteBoards[i][j].board.updateArea.num = 0;
		  ifGuessBlackBoards[i][j].board.updateArea.num = 0;
	  }
    }
  }

  return;
}
