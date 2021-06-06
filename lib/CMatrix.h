#pragma once

#include "CExceptions.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <math.h>
#define MPtr std::shared_ptr<CMatrix>

/*
  CMatrix class
  - has all main matrix functions/operations
  - has 2 derived classes: CDense and CSparse 
    which differ mainly in storing and getting matrix values
*/


bool ShouldBeDense (const std::string & src);
bool Compare (float left, float right);

class CMatrix {
  public:
    CMatrix (int m, int n, bool is_matrix);
    virtual float GetValue (int m, int n) const = 0;
    virtual void  SetValue (float value, int m, int n) = 0;
    virtual CMatrix * Clone () const = 0;
    bool IsSet () const;
    //main commands:
    virtual MPtr Transpose () const = 0;
    MPtr Inverse();
    std::pair<MPtr, float> GEM ();
    int Rank ();
    float Determinant ();
    friend MPtr Split (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    friend MPtr Merge (const CMatrix & left, const CMatrix & right);


    virtual ~CMatrix () {}
    // friend class CDense;
    // friend class CSparse;
    void Read      (std::istream & in );

  private:
  // functions used in main methods:
    bool  ColumnIsZero    (int starting_row, int column) const; 
    int   ColumnNotZero   (int starting_row, int column) const; 
    float RowsMinus       (int starting_row, int column);
    int   FindMaxValInCol (int col, int starting_row) const;
    void  MultRow         (int row, float factor);
    void  MultAdd         (int i, int j, float factor);
    bool  leftHalfIsId    () const;
    virtual void SwapRows (int row1, int row2) = 0;


    friend bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op);
    friend int  CountSum             (const CMatrix & left, const CMatrix & right, char op);
    friend int  CountMult            (const CMatrix & left, const CMatrix & right);
    friend bool MergeShouldBeDense   (const CMatrix & left, const CMatrix & right);
    friend bool SplitShouldBeDense   (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    

    friend std::istream & operator >>
     (std::istream & in, std::shared_ptr<CMatrix> & matrix);
    void         ReadSize  (std::string & src );
    friend std::ostream & operator << 
     (std::ostream & out, const CMatrix & matrix);

  protected:
  //is_matrix is flag if matrix is float value (which is represented as matrix)
    friend class CCommands;
    friend class OperatorToken;
    int m_m;
    int m_n;
    bool is_matrix;
};