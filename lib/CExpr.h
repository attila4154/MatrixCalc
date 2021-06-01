#pragma once

#include "CToken.h"
#include "CDense.h"
#include "define.h"
#include <vector>
#include <memory>

using namespace std;
//================================================================
class CExpr {
  public:
    CExpr () = default;
    CExpr (shared_ptr<CMatrix> matrix);
    void   ReadExpr (istream & in, CMemory & matrices);
    CMatrix * Evaluate(CMemory & matrices);
    void Transpose ();
    shared_ptr<Token> GetMatrix ();
    void ReadCommand (int command, std::istream & in, CMemory & matrices); 
    friend std::ostream & operator << (std::ostream & out, const CExpr & expr);
    
  private:
    void ParseExpr (istream & in, CMemory & matrices);
    void TurnToRPN (void);

    enum ExprCommands {
      NOTSET,
      MERGE, 
      SPLIT
    };
    vector <shared_ptr<Token>> tokens;
};
//================================================================