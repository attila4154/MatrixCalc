#pragma once

#include "CMatrix.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <iostream>

#define DensePtr std::shared_ptr<CDense>

class CDense : public CMatrix {
  public:
    CDense               ();
    CDense               (int m, int n);
    CDense               (float number);
    CDense               (const CDense & other);
    CDense & operator =  (const CDense & other);

    friend DensePtr operator + (const CDense & left, const CDense & right); 
    void FindSize        (std::string & src);
    std::shared_ptr<CDense> operator - (const CDense & other) const;
    bool NotSet          (void) const;
    friend std::shared_ptr<CDense> operator * (const CDense & left, const CDense & right);
    CMatrix * Clone () const;
    float GetValue (int m, int n) const ;
    void  SetValue (float value, int m, int n) ;

    void SwapRows (int row1, int row2);
    float RowsMinus (int starting_row, int column);

    MPtr Transpose () const;

    // void Print (std::ostream & out) const;
    // void Read  (std::istream & in);
    ~CDense();
//-----------------------------------------------------------------
  private:
    std::vector <std::vector <float>> m_matrix;
};
