#pragma once
// #include "CDense.h"
// #include "CSparse.h"
// #include "../src/CMatrix.cpp"
#include "CExceptions.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <math.h>

#define MPtr std::shared_ptr<CMatrix>

bool ShouldBeDense (const std::string & src);

// class CDense ;
// class CSparse;
class CMatrix {
  public:
    CMatrix (int m, int n, bool is_matrix);
    CMatrix () = default;
    virtual CMatrix * Clone () = 0;
    friend MPtr operator + (const CMatrix & left, const CMatrix & right);
    friend MPtr operator - (const CMatrix & left, const CMatrix & right);
    friend MPtr operator * (const CMatrix & left, const CMatrix & right);
    virtual float GetCoord (int m, int n) const = 0;
    virtual void  SetCoord (float value, int m, int n) = 0;
    virtual void Print     (std::ostream & out) const = 0;
    void         ReadSize  (std::string & src );
    virtual void Read      (std::istream & in ) = 0;
    bool IsSet () const;
    virtual MPtr Transpose () = 0;
    std::pair<MPtr, float> GEM ();
    bool ColumnIsZero (int starting_row, int column) const; 
    int ColumnNotZero (int starting_row, int column) const; 
    virtual void SwapRows (int row1, int row2) = 0;
    float RowsMinus (int starting_row, int column);

    int Rank ();

    MPtr Inverse();
    int FindMaxValInCol (int col, int starting_row) const;
    void MultRow (int row, float factor);
    void MultAdd (int i, int j, float factor);
    bool leftHalfIsId ()const;

    float Determinant ();


    friend MPtr Merge (const CMatrix & left, const CMatrix & right);
    friend MPtr Split (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    friend bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op);
    friend int CountSum (const CMatrix & left, const CMatrix & right, char op);
    friend int CountProduct (const CMatrix & left, const CMatrix & right);
    friend bool MergeShouldBeDense (const CMatrix & left, const CMatrix & right);
    friend bool SplitShouldBeDense (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    

    friend std::istream & operator >>
     (std::istream & in, std::shared_ptr<CMatrix> & matrix);
    friend std::ostream & operator << 
     (std::ostream & out, const CMatrix & matrix);

    virtual ~CMatrix () {}

  protected:
    int m_m;
    int m_n;
    bool is_matrix;
};