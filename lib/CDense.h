#pragma once

// #include "CMatrix.h"
// #include "CSparse.h"
#include "CExceptions.h"
// #include "../src/CDense.cpp"
#include <string>
#include <vector>
#include <iostream>
#include <memory>


class CDense {
public:
    CDense               ();
    CDense               (int m, int n);
    CDense               (const CDense & other);
    CDense & operator =    (const CDense & other);
    // CDense operator +    (const CDense & other) const;
    std::shared_ptr<CDense> operator + (const CDense & other) const;
    void FindSize        (std::string & src);
    CDense operator -    (const CDense & other) const;
    bool NotSet          (void) const;
    // CMatrix & operator * (const CMatrix & other) const;
    friend CDense operator * (float alpha, const CDense & matrix);
    friend CDense operator * (const CDense & left, const CDense & right);
    // CMatrix & Split      (int m, int n);
    // CMatrix & Merge      (const CMatrix & other) const;
    // CMatrix & Transpose  ();
    // CMatrix & Inverse    ();
    // CMatrix & gem  ();
    // int       Rank () const;
    // float     Determinant () const;

    friend std::istream & operator >> (std::istream & in, CDense & matrix);
    friend std::ostream & operator << (std::ostream & out, const CDense & matrix);
//    friend void GEM         (const CMatrix & matrix);
//    friend void GEM_comment (const CMatrix & matrix);
    void Print (std::ostream & out) const;
    ~CDense();
//-----------------------------------------------------------------
  private:
    std::vector <std::vector <float>> m_matrix;
    int m_m;
    int m_n;
};
