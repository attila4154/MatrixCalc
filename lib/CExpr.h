#pragma once

#include "MatrixFunctions.h"
#include "CMatrixCommands.h"
#include "CMatrix.h"
#include "CToken.h"
#include "CDense.h"
#include "define.h"
#include <vector>
#include <memory>


//================================================================
class CExpr {
  public:
    CExpr ();
    CExpr (MPtr matrix);
    int Size () const;
    bool Contains (const std::string & varName) const;
    bool ContainsVariable () const;
    void ReadExpr (std::istream & in, CMemory & matrices);
    MPtr Evaluate (CMemory & matrices);
    std::shared_ptr<Token> GetMatrix ();
    void ExecuteCommand (int command, std::istream & in, CMemory & matrices); 
    int  TryToReadCommand   (std::istream & in); 
    friend std::ostream & operator << (std::ostream & out, const CExpr & expr);
    
  private:
    void ParseExpr (std::istream & in, CMemory & matrices);
    void TurnToRPN (void);
    
    // void GemExpr (std::istream & in, CMemory & matrices);
    void MergeExpr (std::istream & in, CMemory & matrices);
    void SplitExpr (std::istream & in, CMemory & matrices);
    void TransposeExpr (std::istream & in, CMemory & matrices);

    static void ReadCoord (std::istream & in, int & m, int & n);

    enum ExprCommands {
      NOTSET,
      MERGE, 
      SPLIT,
      TRANSPOSE
    };
    std::vector <std::shared_ptr<Token>> tokens;
};
//================================================================