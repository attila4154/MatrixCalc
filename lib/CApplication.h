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

#include <fstream>
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
    RANK,
    WRONG_COM,
    DETERMINANT,
    HELP,
    EXIT
};
//----------------------------------------------------------------------
class CApplication {
  public:
    void        Run         ();
  private:
    int         GetCommand  (std::istream & in);
    std::string ReadVar     (std::istream & in);
    void        ReadMatrix  (std::istream & in, MPtr & matrix, int m, int n);
    void        ReadSize    (std::istream & in, int & m, int & n);
    void        Execute     (std::istream & in, CMemory & matrices);
    void        Print       (std::istream & in);
    void        Gem         (std::istream & in);
    void        Scan        (std::istream & in);
    void        ReadFromFile(const std::string & in, std::string varName);
    void        ReadExpr    (std::istream & in);
    void        Evaluate    (std::istream & in);
    void        Transpose   (std::istream & in);
    void        Rank        (std::istream & in);
    void        Inverse     (std::istream & in);
    void        Determinant (std::istream & in);
    void        Help        ();
    
    CMemory matrices;
};