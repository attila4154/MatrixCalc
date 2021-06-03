#pragma once
#include "CMatrix.h"
// #include <vector>
#include <map>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <utility>

class CSparse : public CMatrix {
  public:
    CSparse ();
    CSparse (int m, int n);
    CSparse (const CSparse & other);
    CMatrix * Clone ();

    MPtr Transpose ();

    float GetCoord (int m, int n) const ;
    void  SetCoord (float value, int m, int n) ;

    void SwapRows (int row1, int row2);
    float RowsMinus (int starting_row, int column);

    void Print (std::ostream & out) const ;
    void Read  (std::istream & in ) ;


    ~CSparse ();

  private:
    /*                        rows  cols values
                                ^     ^    ^
                                |     |    |            */
    // std::vector <std::tuple <int, int, double> > values;
    std::map <std::pair<int, int>, float > m_matrix;
};