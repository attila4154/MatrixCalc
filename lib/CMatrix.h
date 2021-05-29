#pragma once
#include "CDense.h"
// #include "../src/CMatrix.cpp"
#include <iostream>
#include <memory>

#define MPtr std::shared_ptr<CMatrix>

class CMatrix {
  public:
    CMatrix (int m, int n);
    CMatrix () = default;
    virtual CMatrix * Clone () = 0;

    // friend MPtr operator +(const CMatrix & left, const CMatrix & right);
    friend MPtr operator +(const CMatrix & left, const CMatrix & right);
    friend MPtr operator -(const CMatrix & left, const CMatrix & right);
    // friend MPtr operator *(const CMatrix & left,
    //                        const CMatrix & right);
    // virtual float GetCoord (int m, int n) = 0;
    virtual void Print (std::ostream & out) const = 0;
    virtual void Read  (std::istream & in ) = 0;


    friend std::istream & operator >>
     (std::istream & in, CMatrix & matrix);
    friend std::ostream & operator << 
     (std::ostream & out, const CMatrix & matrix);

    virtual ~CMatrix () {}

  protected:
    int m_m;
    int m_n;
};