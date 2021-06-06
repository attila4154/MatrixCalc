#pragma once 


#include "CExpr.h"
#include "MatrixFunctions.h"
#include "CMatrixCommands.h"
// #include "../src/CToken.cpp"
// #include "../src/CExpr.cpp"
#include "CDense.h"
#include "CSparse.h"
#include "CMatrix.h"
// #include "../src/CMatrix.cpp"
#include "CExceptions.h"
// #include "../src/CDense.cpp"
// #include "../src/CSparse.cpp"
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

    void        Execute     (std::istream & in);
    int         GetCommand  (std::istream & in);
    void        SwitchCommand(std::istream & is, int command, 
                                   const std::string & str);
    void        Scan        (std::istream & in);
    void        Print       (std::istream & in);
    void        ReadExpr    (std::istream & in);
    void        Evaluate    (std::istream & in);
    void        Gem         (std::istream & in);
    void        Transpose   (std::istream & in);
    void        Rank        (std::istream & in);
    void        Inverse     (std::istream & in);
    void        Determinant (std::istream & in);
    std::string ReadVar     (std::istream & in);
    // void        ReadSize    (std::istream & in, int & m, int & n);
    void        ReadFromFile(const std::string & in, std::string varName);
    void        Help        ();
    
    
    CMemory matrices;
};