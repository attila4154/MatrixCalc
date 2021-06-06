#pragma once
#include "CMatrix.h"
#include <map>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <utility>
#include <iostream>

/*
  CSparse represents sparse matrix
  - all data is stored as map of pair (int, int) and float value 
    where pair of ints is coordinates of the non-zero value
  +: stores data much more efficient for matrices with many non-zero values
       than 'regular' 2d matrix;
     every non-zero value takes 12 bytes + some overhead for map and pair
     sparse matrix does not store zero values 
  -: has logarithmic complexity for both getting and setting values
     while 'regular' dense matrix has constant
*/


class CSparse : public CMatrix {
  public:
    CSparse ();
    CSparse (int m, int n);
    CSparse (const CSparse & other);
    CMatrix * Clone () const;

    MPtr Transpose () const;

    float GetValue (int m, int n) const ;
    void  SetValue (float value, int m, int n) ;

    void  SwapRows (int row1, int row2);
    float RowsMinus (int starting_row, int column);
    // void  Read  (std::istream & in ) ;


    ~CSparse ();

  private:
  //                    rows  cols values
    std::map <std::pair<int, int>, float > m_matrix;
};