#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <queue>
#include <stack>

#include "CMatrix.h"
#include "CDense.h"
#include "CSparse.h"
#include "define.h"

class CExpr;

/*
    token class has 4 derived classes:
    -matrix, variable, operator, brackets
*/

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
    Token                           (int t);
    int                   GetType   (void)               const;
    virtual void          Print     (std::ostream & out) const = 0; 
    virtual MPtr          Value     (CMemory & matrices);
    virtual std::string * GetName   ();
    // virtual void          Transpose ();
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
    void Print (std::ostream & out) const;
    ~MatrixToken () {}
  private:
    std::shared_ptr<CMatrix> matrix;
};
//=========================================================
class OperatorToken : public Token {
public:
    OperatorToken (char sign, int prec);
    void Print (std::ostream & out) const;
    int Precedence ();
    std::shared_ptr<MatrixToken> Calculate 
    (MPtr left, MPtr right, CMemory & matrices);
    // friend MPtr operator + (const CMatrix & left, const CMatrix & right);
    MPtr Sum  (const CMatrix & left, const CMatrix & right);
    MPtr Sub  (const CMatrix & left, const CMatrix & right);
    MPtr Mult (const CMatrix & left, const CMatrix & right);
    // friend MPtr operator - (const CMatrix & left, const CMatrix & right);
    // friend MPtr operator * (const CMatrix & left, const CMatrix & right);
private:
    char op;
    int precedence;
};
//=========================================================
class Brackets : public Token {
  public:
    Brackets   (char b);
    void Print (std::ostream & out) const;
  private:
    bool isLeftBr;
};
//=========================================================
/*
  variable class contains only name of the variable
*/
class Variable : public Token {
  public:
    Variable              (const std::string & name);
    // MPtr Value            (CMemory & matrices) override; 
    std::string * GetName () override;
    void Print            (std::ostream & out) const;
  private:
    std::string varName;
};
//--------------------------------------------------------
