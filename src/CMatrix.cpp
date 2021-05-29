#pragma once
#include "../lib/CMatrix.h"
// #include "CDense.cpp"

CMatrix::CMatrix (int m, int n) : m_m (m), m_n (n) {}


MPtr operator + (const CMatrix & left, const CMatrix & right) {
    // std::shared_ptr<CDense> temp = dynamic_cast<const CDense &>(left) +
    //                                dynamic_cast<const CDense &>(right) ;
    return std::dynamic_pointer_cast<CMatrix>(dynamic_cast<const CDense &>(left) +
                                   dynamic_cast<const CDense &>(right));
}

MPtr operator - (const CMatrix & left, const CMatrix & right) {
    std::shared_ptr<CDense> temp = dynamic_cast<const CDense &>(left) -
                                   dynamic_cast<const CDense &>(right) ;
    return std::dynamic_pointer_cast<CMatrix> (temp);
}

std::istream & operator >> (std::istream & in, CMatrix & matrix){
    matrix.Read (in);
    return in;
}

std::ostream & operator << (std::ostream & out, const CMatrix & matrix){
    matrix.Print (out);
    return out;
}