#pragma once
#include <string>
#include "CDense.h"
#include "CSparse.h"

// bool ShouldBeDense (const std::string & src);
void ReadMatrix  (std::istream & in, MPtr & matrix);
// void ReadSize (std::istream & in, int & m, int & n);
void SetMatrixSize (MPtr & matrix, const std::string & src);
void SetMatrixSize (MPtr & matrix, std::istream & in);