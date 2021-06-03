#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <stack>

#include "CDense.h"
#include "CMatrix.h"
#include "define.h"
#include "../src/CDense.cpp"

class CExpr;

class Token {
  public:
    enum TokenType {
        Nothing,
        Operator,
        MATRIX,
        Number,
        LeftBracket,
        RightBracket,
        Variable
    };
    Token                        (int t);
    int           GetType        (void)               const;
    virtual void  Print          (std::ostream & out) const = 0; 
    virtual MPtr Value (CMemory & matrices);
    virtual std::string * GetName ();
    virtual void Transpose ();
    friend std::ostream & operator << (std::ostream & out, const Token & t);
  protected:
    int type = TokenType::Nothing;
};
//=========================================================
class MatrixToken : public Token {
  public:
    MatrixToken (const CMatrix & m);
    MatrixToken (CMatrix * m);
    MatrixToken (std::shared_ptr<CMatrix> m);
    MatrixToken (float number);
    MPtr Value (CMemory & matrices) override;
    std::shared_ptr<CMatrix> SharedValue ();
    void Print (std::ostream & out) const;
    void Transpose () override;
    ~MatrixToken () {}
  private:
    std::shared_ptr<CMatrix> matrix;
};
//=========================================================
class Operator : public Token {
public:
    Operator (char sign, int prec);
    void Print (std::ostream & out) const;
    int Precedence ();
    std::shared_ptr<MatrixToken> Calculate 
    (std::shared_ptr<Token> left, std::shared_ptr<Token> right, CMemory & matrices);
private:
    char op;
    int precedence;
};
//=========================================================
class Brackets : public Token {
  public:
    Brackets (char b);
    void Print (std::ostream & out) const;
  private:
    bool leftBr;
};
//=========================================================
class Variable : public Token {
  public:
    Variable (const std::string & name, std::shared_ptr<CMatrix> matrix);
    Variable (const std::string & name, std::shared_ptr<CExpr>  matrix,
              CMemory & matrices);
    Variable (const std::string & name, nullptr_t);
    MPtr Value (CMemory & matrices);
    const std::string & Name () const ; 
    std::string * GetName ();
    void Print (std::ostream & out) const;
  private:
    std::string varName;
    std::shared_ptr<CExpr> expression;
    std::shared_ptr<CMatrix> evaluatedExpr;
};
//--------------------------------------------------------
