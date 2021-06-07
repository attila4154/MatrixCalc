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
    /// default constructor
    CExpr ();
    /// "wraps" matrix into expression
    CExpr (MPtr matrix);
    /**
     * @param[out] returns number of tokens of expression
     */
    int Size () const;
    /**
     * returns true if expression contains variable with given name\n
     * used for analyzing if expression is recurisive
     */
    bool Contains (const std::string & varName) const;
    /**
     *  returns true if expression contains at least 1 variable\n
     *  used for analyzing if expression can be evaluated right after it was read
     */
    bool ContainsVariable () const;
    /** 
     * method that reads expression that looks like "x = ..."
     */
    void ReadExpr (std::istream & in, CMemory & matrices);
    /**
     * evalautes expression
     * @param[out] matrix: evaluated matrix
     * @param[in] matrices: map of all expression used for evaluating expressions with variables 
     */
    MPtr Evaluate (CMemory & matrices);
    /// prints out expression in Reversed Polish Notation
    friend std::ostream & operator << (std::ostream & out, const CExpr & expr);
    
  private:
    /**
     * executes command from input stream and add new matrix 
     */
    void ExecuteCommand (int command, std::istream & in, CMemory & matrices); 
    /// method to parse expression from input into tokens
    void ParseExpr     (std::istream & in, CMemory & matrices);

    /** 
        method that turns expression into reversed polish notation using [Shunting Yard Algorithm]
        (https://en.wikipedia.org/wiki/Shunting-yard_algorithm)
     */
    void TurnToRPN     (void);

    /// initializes expression as merged matrix from 2 given expressions/matrices
    void MergeExpr     (std::istream & in, CMemory & matrices);

    /// initializes expression as split  matrix from 1 given expression/matrix
    void SplitExpr     (std::istream & in, CMemory & matrices);

    /// initializes expression as transposed matrix
    void TransposeExpr (std::istream & in, CMemory & matrices);

    /// initializes expression as inversed matrix 
    void InverseExpr   (std::istream & in, CMemory & matrices);
    /**
     * function that decided if user input was matrix command or mathematical expression
     */
    static int  TryToReadCommand  (std::istream & in); 
    /** 
     * enum value that represents read expression/command\n
     * where notset is maths expression
     */
    enum ExprCommands {
      NOTSET,
      MERGE, 
      SPLIT,
      TRANSPOSE,
      INVERSE
    };
    std::vector <std::shared_ptr<CToken>> tokens;
};
//================================================================