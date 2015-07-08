#ifndef _DEPENDENCY_H_
#define _DEPENDENCY_H_

#include "puzzle.h"
// Sizeof( DependencyTable ) = 5032 bytes
struct DependencyTable{
    LineMask pointTables[P_SIZE][P_SIZE];
    LineMask updateTable;
    LineMask tempUpdateTable;
    LineMask thisUpdateTable;
    LineMask thisTempTable;
};

#endif