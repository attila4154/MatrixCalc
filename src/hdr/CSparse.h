#pragma once
#include "CMatrix.h"
#include <map>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <utility>
#include <iostream>



/**
  CSparse represents sparse matrix:\n
  * All data is stored as map of pair (int, int) and float value 
    where pair of ints is coordinates of the non-zero value\n
  * Every non-zero value takes 12 bytes + some overhead for map and pair\n
  +: 
  * Stores data much more efficient for matrices with many non-zero values
       than 'regular' 2d matrix;\n
  * Sparse matrix does not store zero values\n
  -: 
  *Has logarithmic complexity for both getting and setting values
     while 'regular' dense matrix has constant
*/
class CSparse : public CMatrix {
  public:
    CSparse ();
    CSparse (int m, int n);
    CSparse (const CSparse & other);
    CMatrix * Clone () const;

    MPtr Transpose () const;
    void  SwapRows (int row1, int row2);

    ~CSparse ();

  private:
    float GetValue (int m, int n) const ;
    void  SetValue (float value, int m, int n) ;
  //                    rows  cols values
    std::map <std::pair<int, int>, float > m_matrix;
};