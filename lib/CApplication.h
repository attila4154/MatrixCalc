#pragma once 

#include "CExpr.h"
#include "../src/CToken.cpp"
#include "../src/CExpr.cpp"
#include "CDense.h"
#include "CSparse.h"
#include "CMatrix.h"
#include "../src/CMatrix.cpp"
#include "CExceptions.h"
#include "../src/CDense.cpp"
#include "../src/CSparse.cpp"
#include "define.h"

#include <iostream>
#include <string>
#include <sstream>

//======================================================================
enum commands {
    PRINT = 0,
    SCAN,
    GEM,
    TRANSPOSE,
    READEXPR,
    EVALUATE,
    INVERSE,
    WRONG_COM,
    EXIT
};
//----------------------------------------------------------------------
class CApplication {
  public:
    void        Run         ();
    int         GetCommand  (std::istream & in);
    std::string ReadVar     (std::istream & in);
    void        ReadMatrix  (std::istream & in, MPtr & matrix, int m, int n);
    void        ReadSize    (std::istream & in, int & m, int & n);
    void        Execute     (std::istream & in, CMemory & matrices);
    // void        ReadCommand (std::istream & in, CMemory & matrices);

  private:
    void Print    (std::istream & in);
    void Gem      (std::istream & in);
    void Scan     (std::istream & in);
    void ReadExpr (std::istream & in);
    void Evaluate (std::istream & in);
    void Transpose(std::istream & in);
    
    CMemory matrices;
};