#include "Dt.h"
void UglyDt( const Puzzle problem, const Board solution) 
{
  // set 2-pair cell
  printf("digraph G {\n") ;
  Board resumeBoard = solution ;
  Board flowBoard = solution ;
  for ( int i = 0 ; i < S_SIZE ; i++ )
  {
    for ( int j = i + 1 ; j < S_SIZE ; j++ )
    {
	  // cerr << "i:" << i << " j:" << j << endl;
      int i_1 = i / P_SIZE ;
      int j_1 = i % P_SIZE ;
      int i_2 = j / P_SIZE ;
      int j_2 = j % P_SIZE ;
      if ( getSquare( i_1, j_1, solution ) == SQUARE_UNKNOWN &&
           getSquare( i_2, j_2, solution ) == SQUARE_UNKNOWN
         )
      {
        // BB        
        flowBoard = resumeBoard ;
        setSquare( i_1, j_1, flowBoard, SQUARE_SPACE ) ;
        setSquare( i_2, j_2, flowBoard, SQUARE_SPACE ) ;
        if ( !IsLegal( problem, flowBoard ) )
          printf("\"(%d, %d)\" -> \"-(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
        
        // BW
        flowBoard = resumeBoard ;
        setSquare( i_1, j_1, flowBoard, SQUARE_SPACE ) ;
        setSquare( i_2, j_2, flowBoard, SQUARE_BLOCK ) ;
        if ( !IsLegal( problem, flowBoard ) )
          printf("\"(%d, %d)\" -> \"(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
       
        // WB
        flowBoard = resumeBoard ;
        setSquare( i_1, j_1, flowBoard, SQUARE_BLOCK ) ;
        setSquare( i_2, j_2, flowBoard, SQUARE_SPACE ) ;
        if ( !IsLegal( problem, flowBoard ) )
          printf("\"-(%d, %d)\" -> \"-(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
        
        // WW
        flowBoard = resumeBoard ;
        setSquare( i_1, j_1, flowBoard, SQUARE_BLOCK ) ;
        setSquare( i_2, j_2, flowBoard, SQUARE_BLOCK ) ;
        if ( !IsLegal( problem, flowBoard ) )
          printf("\"-(%d, %d)\" -> \"(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
      }
    }
  }
   printf("}\n") ;
}

bool CheckMaxFlow(long idealFlow, std::stringstream& sStream) 
{
  using namespace boost ;

  typedef adjacency_list_traits<vecS, vecS, directedS> Traits;
  typedef adjacency_list<listS, vecS, directedS, 
    property<vertex_name_t, std::string>,
    property<edge_capacity_t, long,
    property<edge_residual_capacity_t, long,
    property<edge_reverse_t, Traits::edge_descriptor> > >
  > Graph;

  Graph g;

  property_map<Graph, edge_capacity_t>::type 
    capacity = get(edge_capacity, g);
  property_map<Graph, edge_reverse_t>::type 
    rev = get(edge_reverse, g);
  property_map<Graph, edge_residual_capacity_t>::type 
    residual_capacity = get(edge_residual_capacity, g);

  Traits::vertex_descriptor s, t;
  
  read_dimacs_max_flow(g, capacity, rev, s, t, sStream);
  
  return idealFlow == push_relabel_max_flow(g, s, t);

}

bool IsLegal( const Puzzle problem, const Board solution ) 
{
  std::stringstream sStream ;
  std::string sStringBuffer = "" ;
  char tmp[100] ;
  sprintf( tmp, "p max    %d    %d\n", P_SIZE * 2 + 2, S_SIZE + P_SIZE * 2 /*- solution.numOfSquareOnBoard*/) ;
  sStringBuffer.append(tmp) ;
  sprintf( tmp, "n    1    s\nn    %d    t\n", P_SIZE * 2 + 2 ) ;
  sStringBuffer.append(tmp) ;
  long idealFlow = 0 ;
  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    int numOfBlack = 0 ;
    for ( int j = 0 ; j < P_SIZE ; j++ )
      if ( getSquare( i, j, solution) == SQUARE_BLOCK ) 
        numOfBlack++ ;
    sprintf( tmp, "a    %d    %d    %d\n", 1, i + 2, problem.mLines[i].mSum[problem.mLines[i].mCount] - numOfBlack ) ;
    sStringBuffer.append(tmp) ;
    idealFlow += (int) problem.mLines[i].mSum[problem.mLines[i].mCount] - numOfBlack ;
  }

  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    int sourceNode = i + 2 ;
    for ( int j = 0 ; j < P_SIZE ; j++ )
    {
      int destNode = P_SIZE + 2 + j ;
      if ( getSquare( i, j, solution ) == SQUARE_UNKNOWN )
      {
        sprintf( tmp, "a    %d    %d    %d\n", sourceNode, destNode, 1 ) ;
        sStringBuffer.append(tmp) ;
      }
      else
      {
        sprintf( tmp, "a    %d    %d    %d\n", sourceNode, destNode, 0 ) ;
        sStringBuffer.append(tmp) ;
      }

    }
  }

  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    int numOfBlack = 0 ;
    for ( int j = 0 ; j < P_SIZE ; j++ )
      if ( getSquare( j, i, solution) == SQUARE_BLOCK ) 
        numOfBlack++ ;
    sprintf( tmp, "a    %d    %d    %d\n",  P_SIZE + i + 2, P_SIZE * 2 + 2, problem.mLines[P_SIZE+i].mSum[problem.mLines[P_SIZE+i].mCount] - numOfBlack) ;
    sStringBuffer.append(tmp) ;
  }
  sStream << sStringBuffer ;
  return CheckMaxFlow(idealFlow, sStream) ;
}

bool FindNode( bool aMatrix[S_SIZE * 2][S_SIZE * 2], int nSource, int nGoal) 
{
  if ( aMatrix[nSource][nGoal] )
    return true ;

  for ( int i = 0 ; i < S_SIZE * 2 ; i++ )
    aMatrix[i][nSource] = false ;

  for ( int i = 0 ; i < S_SIZE * 2 ; i++ )
  {
    if ( aMatrix[nSource][i] )
    {
      aMatrix[nSource][i] = false ;
      bool isFind = FindNode( aMatrix, i, nGoal ) ;
      if ( isFind )
        return true ;
    }
  }

  return false ;
}

int Solve2SatOrDT( const Puzzle problem, Board& solution ) 
{
  // 0 ~ S_SIZE           WHITE
  // S_SIZE ~ 2 * S_SIZE  BLACK
  bool isUpdate = true ;
  bool aMatrix[S_SIZE * 2][S_SIZE * 2] ;
  bool aCopyMatrix[S_SIZE * 2][S_SIZE * 2] ;
  while ( isUpdate )
  {
    isUpdate = false ;
    memset( aMatrix, 0, sizeof(aMatrix) ) ;

#ifdef TWO_SAT
    Find2SATRelations( problem, solution, aMatrix ) ;
    for ( int i = 0 ; i < S_SIZE ; i++ )
    {
      memcpy( aCopyMatrix, aMatrix, sizeof(aCopyMatrix) ) ;
      bool isFindA = FindNode( aCopyMatrix, i, i + S_SIZE ) ;
      memcpy( aCopyMatrix, aMatrix, sizeof(aCopyMatrix) ) ;
      bool isFindB = FindNode( aCopyMatrix, i + S_SIZE, i ) ;
    
      if ( isFindA && isFindB )
      {
        return CONFLICT ;    
      }
      else if ( isFindA )
      {
        isUpdate = true ;
        setAndFlag( i / P_SIZE, i % P_SIZE, solution, SQUARE_SPACE ) ;
      }
    
      if ( isFindB )
      {
        isUpdate = true ;
        setAndFlag( i / P_SIZE, i % P_SIZE, solution, SQUARE_BLOCK ) ;
      }
    }
#else
	// no TWO-SAT
	memcpy( aCopyMatrix, aMatrix, sizeof(aCopyMatrix) ) ;
#endif

#ifdef DT
    if ( !isUpdate ) // 若 2-SAT 有找到,再做一次 2-SAT!! 否則做 DT.
    {
	  // cerr << "MakeDT..." << endl;
      MakeDt( problem, solution, aCopyMatrix ) ;
      bool dtWork = SolveDt(problem, solution, aCopyMatrix) ;
      if ( dtWork )
      {
        int state = lineSolver(problem, solution) ;
        if ( state == SOLVED )
          return SOLVED ;
      }
      else
        return lineSolver(problem, solution) ;
    }
#endif

    int state = lineSolver(problem, solution) ;
    if ( state == SOLVED )
      return SOLVED ;
    else if ( state == CONFLICT )
      return CONFLICT ;
  }

  return UNSOLVED ;
}

extern unsigned char DPtable[P_SIZE+1][(P_SIZE+1)/2+1];
void Find2SATRelations(const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] )
{  
  // set 2-pair cell
  for ( int i = 0 ; i < P_SIZE ; i++ )
  {
    for ( int j = 0 ; j < P_SIZE ; j++ )
    {
      if ( getSquare( i, j, solution ) == SQUARE_UNKNOWN ) 
      {
        for ( int k = j + 1 ; k < P_SIZE ; k++ )
        {
          if ( getSquare( i, k, solution ) == SQUARE_UNKNOWN ) 
          {
            int index1 = i * P_SIZE + j ;
            int index2 = i * P_SIZE + k ;
            // B B
            memset(DPtable,UNSOLVED,sizeof(DPtable));
#ifdef SSE_BOARD
            LineMask_128 settle;
            LineMask_128 line = solution.colString[i] ;
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ j ];
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
#else
            LineMask settle;
            LineMask line = solution.colString[i] ;
	          line &= ~(SHIFT_L(SQUARE_SPACE, j));
	          line &= ~(SHIFT_L(SQUARE_SPACE, k));
#endif


            LineNumbers lineNumber = problem.mLines[i] ;
            if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
              aMatrix[index1 + S_SIZE][index2] = true ;
              aMatrix[index2 + S_SIZE][index1] = true ;
              
              //printf("\"(%d, %d)\"->\"-(%d, %d)\";\n", j, i, k, i) ;
            }

            // B W
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.colString[i] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ j ];
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_SPACE, j));
	          line &= ~(SHIFT_L(SQUARE_BLOCK, k));
#endif

            lineNumber = problem.mLines[i] ;
            if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
              aMatrix[index1 + S_SIZE][index2 + S_SIZE] = true ;
              aMatrix[index2][index1] = true ;
              //printf("\"(%d, %d)\"->\"(%d, %d)\";\n", j, i, k, i) ;
            }

            // W B
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.colString[i] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ j ];
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_BLOCK, j));
	          line &= ~(SHIFT_L(SQUARE_SPACE, k));
#endif
            lineNumber = problem.mLines[i] ;
            if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
              aMatrix[index1][index2] = true ;
              aMatrix[index2 + S_SIZE][index1 + S_SIZE] = true ;
              //printf("\"-(%d, %d)\"->\"-(%d, %d)\";\n", j, i, k, i) ;
            }

            // W W
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.colString[i] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ j ];
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_BLOCK, j));
	          line &= ~(SHIFT_L(SQUARE_BLOCK, k));
#endif
            lineNumber = problem.mLines[i] ;
            if ( !sprintSettle(lineNumber, line, P_SIZE, problem.mLines[i].mCount ,  settle) )
            {
              aMatrix[index1][index2 + S_SIZE] = true ;
              aMatrix[index2][index1 + S_SIZE] = true ;
              //printf("\"-(%d, %d)\"->\"(%d, %d)\";\n", j, i, k, i) ;
            }
          } 
        }

        for ( int k = i + 1 ; k < P_SIZE ; k++ )
        {
          if ( getSquare( k, j, solution ) == SQUARE_UNKNOWN ) 
          {
            // B B
            int index1 = i * P_SIZE + j ;
            int index2 = k * P_SIZE + j ;
            memset(DPtable,UNSOLVED,sizeof(DPtable));
#ifdef SSE_BOARD
            LineMask_128 settle;
            LineMask_128 line = solution.rowString[j] ;
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ i ];
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
#else
            LineMask settle;
            LineMask line = solution.rowString[j] ;
	          line &= ~(SHIFT_L(SQUARE_SPACE, i));
	          line &= ~(SHIFT_L(SQUARE_SPACE, k));
#endif
            LineNumbers lineNumber = problem.mLines[j + P_SIZE] ;
            if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
              aMatrix[index1 + S_SIZE][index2] = true ;
              aMatrix[index2 + S_SIZE][index1] = true ;
              //printf("\"(%d, %d)\"->\"-(%d, %d)\";\n", j, i, j, k) ;
            }

            // B W
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.rowString[j] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ i ];
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_SPACE, i));
	          line &= ~(SHIFT_L(SQUARE_BLOCK, k));
#endif
            lineNumber = problem.mLines[j + P_SIZE] ;
            if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
              aMatrix[index1 + S_SIZE][index2 + S_SIZE] = true ;
              aMatrix[index2][index1] = true ;
              //printf("\"(%d, %d)\"->\"(%d, %d)\";\n", j, i, j, k) ;
            }

            // W B
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.rowString[j] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ i ];
            line &= ~SQUARE_MASK[ SQUARE_SPACE ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_BLOCK, i));
	          line &= ~(SHIFT_L(SQUARE_SPACE, k));
#endif
            lineNumber = problem.mLines[j + P_SIZE] ;
            if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
              aMatrix[index1][index2] = true ;
              aMatrix[index2 + S_SIZE][index1 + S_SIZE] = true ;
              //printf("\"-(%d, %d)\"->\"-(%d, %d)\";\n", j, i, j, k) ;
            }

            // W W
            memset(DPtable,UNSOLVED,sizeof(DPtable));
            line = solution.rowString[j] ;
#ifdef SSE_BOARD
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ i ];
            line &= ~SQUARE_MASK[ SQUARE_BLOCK ][ k ];
#else
	          line &= ~(SHIFT_L(SQUARE_BLOCK, i));
	          line &= ~(SHIFT_L(SQUARE_BLOCK, k));
#endif
            lineNumber = problem.mLines[j + P_SIZE] ;
            if ( !sprintSettle( lineNumber, line, P_SIZE, problem.mLines[j + P_SIZE].mCount ,  settle) )
            {
              aMatrix[index1][index2 + S_SIZE] = true ;
              aMatrix[index2][index1 + S_SIZE] = true ;
              //printf("\"-(%d, %d)\"->\"(%d, %d)\";\n", j, i, j, k) ;
            }
          } 
        }
      }
    }
  }
  
}

void MakeDt(const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] ) 
{
	Board resumeBoard;
	copyBoardWithoutUpdate(resumeBoard, solution);
	Board flowBoard;
	copyBoardWithoutUpdate(flowBoard, solution);

	for ( int i = 0 ; i < S_SIZE ; i++ )
	{
		int i_1 = i / P_SIZE ;
		int j_1 = i % P_SIZE ;
		if ( getSquare( i_1, j_1, solution ) != SQUARE_UNKNOWN ) continue;

		for ( int j = i + 1 ; j < S_SIZE ; j++ )
		{
			int i_2 = j / P_SIZE ;
			int j_2 = j % P_SIZE ;
			if ( getSquare( i_2, j_2, solution ) != SQUARE_UNKNOWN ) continue;

			// BB     
			//fprintf( stderr, "%d %d %d %d\n", j_1, i_1, j_2, i_2 );   
			copyBoardWithoutUpdate(flowBoard, resumeBoard);
			setSquare( i_1, j_1, flowBoard, SQUARE_SPACE ) ;
			setSquare( i_2, j_2, flowBoard, SQUARE_SPACE ) ;
			if ( !IsLegal( problem, flowBoard ) )
			{
				aMatrix[i + S_SIZE][j] = true ;
				aMatrix[j + S_SIZE][i] = true ;
				// printf("\"(%d, %d)\" -> \"-(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
			}

			// BW
			// flowBoard = resumeBoard ;
			copyBoardWithoutUpdate(flowBoard, resumeBoard);
			setSquare( i_1, j_1, flowBoard, SQUARE_SPACE ) ;
			setSquare( i_2, j_2, flowBoard, SQUARE_BLOCK ) ;
			if ( !IsLegal( problem, flowBoard ) )
			{
				aMatrix[i + S_SIZE][j + S_SIZE] = true ;
				aMatrix[j][i] = true ;
				//printf("\"(%d, %d)\" -> \"(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
			}

			// WB
			// flowBoard = resumeBoard ;
			copyBoardWithoutUpdate(flowBoard, resumeBoard);
			setSquare( i_1, j_1, flowBoard, SQUARE_BLOCK ) ;
			setSquare( i_2, j_2, flowBoard, SQUARE_SPACE ) ;
			if ( !IsLegal( problem, flowBoard ) )
			{
				//printf("\"-(%d, %d)\" -> \"-(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
				aMatrix[i][j] = true ;
				aMatrix[j + S_SIZE][i + S_SIZE] = true ;
			}

			// WW
			// flowBoard = resumeBoard ;
			copyBoardWithoutUpdate(flowBoard, resumeBoard);
			setSquare( i_1, j_1, flowBoard, SQUARE_BLOCK ) ;
			setSquare( i_2, j_2, flowBoard, SQUARE_BLOCK ) ;
			if ( !IsLegal( problem, flowBoard ) )
			{
				//printf("\"-(%d, %d)\" -> \"(%d, %d)\";\n", j_1, i_1, j_2, i_2 );
				aMatrix[i][j + S_SIZE] = true ;
				aMatrix[j][i+ S_SIZE] = true ;
			}
			
		}
	}
}

bool SolveDt( const Puzzle problem, Board& solution, bool aMatrix[S_SIZE * 2][S_SIZE * 2] ) 
{
    bool isUpdate = false ;
    bool aCopyMatrix[S_SIZE * 2][S_SIZE * 2] ;
    for ( int i = 0 ; i < S_SIZE ; i++ )
    {
	  // cerr << "SolveDT..." << i << endl;
      memcpy( aCopyMatrix, aMatrix, sizeof(aCopyMatrix) ) ;
      bool isFindA = FindNode( aCopyMatrix, i, i + S_SIZE ) ;
      memcpy( aCopyMatrix, aMatrix, sizeof(aCopyMatrix) ) ;
      bool isFindB = FindNode( aCopyMatrix, i + S_SIZE, i ) ;
    
      if ( isFindA && isFindB )
      {
        fprintf(stderr, "?\n" ) ;
        return false ;    
      }
    
      else if ( isFindA )
      {
        isUpdate = true ;
        setAndFlag( i / P_SIZE, i % P_SIZE, solution, SQUARE_SPACE ) ;
      }
    
      if ( isFindB )
      {
        isUpdate = true ;
        setAndFlag( i / P_SIZE, i % P_SIZE, solution, SQUARE_BLOCK ) ;
      }
    }

    return isUpdate ;
}