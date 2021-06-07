#pragma once
#include <string>
#include "CDense.h"
#include "CSparse.h"
/** 
    function that reads matrix and stores it as Dense or Sparse matrix\n 
    depending on how many 'empty characters' (0) it has                           
*/
void ReadMatrix  (std::istream & in, MPtr & matrix);

/// function that decides whether matrix should be dense or sparse and sets its size
void SetMatrixSize (MPtr & matrix, const std::string & src);

/// function that reads coordinates for split command
void ReadCoord (std::istream & in, int & m, int & n);

/// function that reads size of matrix for split command
void ReadSize  (std::istream & in, int & m, int & n);