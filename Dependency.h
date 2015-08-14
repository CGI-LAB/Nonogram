#ifndef _DEPENDENCY_H_
#define _DEPENDENCY_H_

#include "puzzle.h"
// Sizeof( DependencyTable ) = 5032 bytes
struct DependencyTable{
#ifdef SSE_BOARD
    LineMask_128 pointTables[P_SIZE][P_SIZE];
    LineMask_128 updateTable;
    LineMask_128 tempUpdateTable;
    LineMask_128 thisUpdateTable;
    LineMask_128 thisTempTable;
#else
    LineMask pointTables[P_SIZE][P_SIZE];
    LineMask updateTable;
    LineMask tempUpdateTable;
    LineMask thisUpdateTable;
    LineMask thisTempTable;
#endif
};

#endif