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

class CToken {
  public:
    enum TokenType {
        Nothing,
        Operator,
        Matrix,
        Number,
        LeftBracket,
        RightBracket,
        Variable
    };
    /**
     * constructor
     * @param[in] t: type of token
     */
    CToken                          (int t);
    int                   GetType   (void)               const;
    /**
     * pure virtual method that prints token 
     */
    virtual void          Print     (std::ostream & out) const = 0;
    /**
     * virtual method that returns null pointer to matrix/number\n
     * overrided by CMatrixToken class
     */ 
    virtual MPtr          Value     ();
    /**
     * virtual method that returns null pointer to string of variable\n
     * overrided by CVariableToken class
     */
    virtual std::string * GetName   ();
    /**
     * virtual method that returns precedence\n
     * overrided and used by Operator
     */
    virtual int Precedence () const;
    friend std::ostream & operator << (std::ostream & out, const CToken & t);
  protected:
    int type = TokenType::Nothing;
};
//=========================================================
class CMatrixToken : public CToken {
  public:
  /// constructs matrix token from matrix
    CMatrixToken (const CMatrix & m);

  /// constructs matrix token from pointer to matrix
    CMatrixToken (CMatrix * m);

  /// constructs matrix token from shared_pointer to matrix
    CMatrixToken (std::shared_ptr<CMatrix> m);

  /// constructs number token from number 
    CMatrixToken (float number);
    MPtr Value  () override;
    void Print  (std::ostream & out) const;
    ~CMatrixToken () {}
  private:
    MPtr matrix;
};
//=========================================================
class COperatorToken : public CToken {
public:
    /**
     * operator constructor
     * @param[in] sign: sign of operator
     * @param[in] prec: precedence of operator, where higher value means higher
     *                  precedence 
     */
    COperatorToken (char sign, int prec);
    void Print (std::ostream & out) const;
    int Precedence () const override;
    /**
     * method that calculates product of sum/subtraction/multiplication of 2 matrices
     * @param[in] left:  left matrix pointer
     * @param[in] right: right matrix pointer
     * @param[in] matrices: map of matrices/expressions used for findind values of 
     *            variables
     * @param[out] returns shared pointer to calculated matrix token
     */
    std::shared_ptr<CMatrixToken> Calculate 
    (MPtr left, MPtr right, CMemory & matrices);
    MPtr Sum  (const CMatrix & left, const CMatrix & right);
    MPtr Sub  (const CMatrix & left, const CMatrix & right);
    MPtr Mult (const CMatrix & left, const CMatrix & right);
private:
    char op;
    int precedence;
};
//=========================================================
class CBracketsToken : public CToken {
  public:
    CBracketsToken   (char b);
    void Print (std::ostream & out) const;
  private:

  /// flag if bracket is left or right
    bool isLeftBr;
};
//=========================================================
class CVariableToken : public CToken {
  public:
    CVariableToken        (const std::string & name);

    /// returns pointer to kept variable name
    std::string * GetName () override;

    /// prints variable's name
    void Print            (std::ostream & out) const;
  private:
  /// variable token contains only name of variable
    std::string varName;
};
//--------------------------------------------------------
