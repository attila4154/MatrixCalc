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

    void Transpose ();

    float GetCoord (int m, int n) const ;
    void  SetCoord (float value, int m, int n) ;

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