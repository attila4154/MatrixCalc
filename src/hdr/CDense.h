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
    CDense                 ();
    CDense                 (int m, int n);
    CDense                 (float number);
    CDense                 (const CDense & other);
    CMatrix * Clone        () const override;
    void      SwapRows     (int row1, int row2) override;
    MPtr      Transpose    () const override;

    /// pushes *value* at the end of *row*
    void      ReadRowValue (int row, float value);

    /// sets number of columns
    void      SetCols      (int columns);

    ~CDense();
//-----------------------------------------------------------------
  private:
    void      SetValue     (float value, int m, int n) override;
    float     GetValue     (int m, int n) const override;
  /** Dense matrix is represented as vector of vector of floats\n
   * * setting and getting values is constant
   */
    std::vector <std::vector <float>> m_matrix;
};
