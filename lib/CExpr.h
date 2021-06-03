#pragma once

#include "CToken.h"
#include "CDense.h"
#include "define.h"
#include <vector>
#include <memory>

//================================================================
class CExpr {
  public:
    CExpr () = default;
    CExpr (MPtr matrix);
    void   ReadExpr (std::istream & in, CMemory & matrices);
    MPtr Evaluate(CMemory & matrices);
    void Transpose ();
    std::shared_ptr<Token> GetMatrix ();
    void ReadFullCommand (int command, std::istream & in, CMemory & matrices); 
    int  TryToReadCommand   (std::istream & in); 
    friend std::ostream & operator << (std::ostream & out, const CExpr & expr);
    
  private:
    void MergeCom (std::istream & in, CMemory & matrices);
    void SplitCom (std::istream & in, CMemory & matrices);
    void TransposeCom (std::istream & in, CMemory & matrices);
    void ParseExpr (std::istream & in, CMemory & matrices);
    void TurnToRPN (void);

    enum ExprCommands {
      NOTSET,
      MERGE, 
      SPLIT,
      TRANSPOSE,
      GEM
    };
    std::vector <std::shared_ptr<Token>> tokens;
};
//================================================================