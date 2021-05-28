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
    CExpr (shared_ptr<CDense> matrix);
    void   ReadExpr (istream & in, CMemory & matrices);
    CDense Evaluate(CMemory & matrices);
    shared_ptr<Token> GetMatrix ();
    friend std::ostream & operator << (std::ostream & out, const CExpr & expr);
    
  private:
    void ParseExpr (istream & in, CMemory & matrices);
    void TurnToRPN (void);

    vector <shared_ptr<Token>> tokens;
};
//================================================================
