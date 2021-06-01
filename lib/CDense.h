#pragma once

#include "CMatrix.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>

#define DensePtr std::shared_ptr<CDense>

class CDense : public CMatrix {
  public:
    CDense               ();
    CDense               (int m, int n);
    CDense               (const CDense & other);
    CDense & operator =    (const CDense & other);

    friend DensePtr operator + (const CDense & left, const CDense & right); 
    void FindSize        (std::string & src);
    std::shared_ptr<CDense> operator - (const CDense & other) const;
    bool NotSet          (void) const;
    friend std::shared_ptr<CDense> operator * (const CDense & left, const CDense & right);
    CMatrix * Clone ();
    float GetCoord (int m, int n) const ;
    void  SetCoord (float value, int m, int n) ;

    void Transpose ();

    // CMatrix & operator * (const CMatrix & other) const;
    // friend CDense operator * (float alpha, const CDense & matrix);
    // CMatrix & Split      (int m, int n);
    // CMatrix & Merge      (const CMatrix & other) const;
    // CMatrix & Transpose  ();
    // CMatrix & Inverse    ();
    // CMatrix & gem  ();
    // int       Rank () const;
    // float     Determinant () const;

    // friend std::istream & operator >> (std::istream & in, CDense & matrix);
    // friend std::ostream & operator << (std::ostream & out,const CDense & matrix);
//    friend void GEM         (const CMatrix & matrix);
//    friend void GEM_comment (const CMatrix & matrix);
    void Print (std::ostream & out) const;
    void Read  (std::istream & in);
    ~CDense();
//-----------------------------------------------------------------
  private:
    std::vector <std::vector <float>> m_matrix;
    // int m_m;
    // int m_n;
};
