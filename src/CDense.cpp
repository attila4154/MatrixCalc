#pragma once

#include "../lib/CDense.h"

//=======================================================================
CDense::CDense               (int m, int n) : CMatrix (m,n, true) {
    m_matrix.resize(m);
    m_matrix.reserve(m);
    for (int i = 0; i < m_m; i++)
        m_matrix[i].resize(m_n);
}
//-----------------------------------------------------------------------
CDense::CDense               () : CMatrix (0,0, true) {
    // m_matrix.resize(m_m);
    // m_matrix.reserve(m_m);
}
//-----------------------------------------------------------------------
CDense::CDense (float number) : CMatrix (1, 1, false) {
    m_matrix.resize(1);
    m_matrix[0].push_back(number);
}
//-----------------------------------------------------------------------
CDense::CDense (const CDense & other) : CMatrix (other.m_m, other.m_n, other.is_matrix) {
    m_matrix.resize(m_m);
    m_matrix.reserve(m_m);
    for (int i = 0; i < m_m; ++i) {
        for (int j = 0; j < m_n; j++)  
        {
            m_matrix[i].push_back(other.m_matrix[i][j]);
        }
    }
}
//-----------------------------------------------------------------------
float CDense::GetCoord (int m, int n) const {
    if (m > m_m || n > m_n) throw WrongDimensions ();
    return m_matrix [m][n];
}   
//-----------------------------------------------------------------------
void  CDense::SetCoord (float value, int m, int n) {
    if (m > m_n || n > m_n) throw WrongDimensions();
    m_matrix[m][n] = value;
}
//-----------------------------------------------------------------------
void CDense::SwapRows (int row1, int row2) {
    std::swap (m_matrix[row1], m_matrix[row2]);
}
//-----------------------------------------------------------------------
MPtr CDense::Transpose () {
    std::shared_ptr<CDense> transposed = std::make_shared<CDense> (m_n, m_m);
    // temp.resize(m_n);
    // for (int i = 0; i < m_n; i++) 
    //     temp[i].resize (m_m);

    for (int i = 0; i < m_m; i++) {
        for (int j = 0; j < m_n; j++) {
            transposed->m_matrix[j][i] = m_matrix [i][j];
        }
    }
    return transposed;
}
//-----------------------------------------------------------------------
DensePtr operator + (const CDense & left, const CDense & right) {
    if (! (left.m_n == right.m_n && left.m_m == right.m_m)) throw WrongFormat();
    std::shared_ptr<CDense> temp = std::make_shared<CDense>(right.m_m, right.m_n);
    for (int i = 0; i < temp->m_m; i++) {
        temp->m_matrix[i].resize(temp->m_n);
        temp->m_matrix[i].reserve(temp->m_n);
    }
    for (int i = 0; i < left.m_m; ++i) {
        for (int j = 0; j < left.m_n; j++)  
        {
            temp ->m_matrix[i][j] =
            left  .m_matrix[i][j] +
            right .m_matrix[i][j]; 
        }
    } 
    return temp;
}
//-----------------------------------------------------------------------
std::shared_ptr<CDense> CDense::operator - (const CDense & other) const {
    if (! (m_n == other.m_n && m_m == other.m_m)) throw WrongFormat();
    std::shared_ptr<CDense> temp = std::make_shared<CDense> (other.m_m, other.m_n);
    for (int i = 0; i < temp->m_m; i++) {
        temp->m_matrix[i].resize(temp->m_n);
        temp->m_matrix[i].reserve(temp->m_n);
    }
    for (int i = 0; i < m_m; ++i) {
        for (int j = 0; j < m_n; j++)  
        {
            temp->m_matrix[i][j] = m_matrix[i][j] - other.m_matrix[i][j]; 
        }
    } 

    return temp;
}
//-----------------------------------------------------------------------
void CDense::Print(std::ostream & out) const {
    for (size_t i = 0; i < m_matrix.size(); ++i) {
        out << '(';
        for (size_t j = 0; j < m_matrix[i].size(); j++)  
            out << m_matrix[i][j] << (j != m_matrix[i].size() - 1 ? " " : "");
        out << ')';
        if (i != m_matrix.size() - 1) out << std::endl;
    } 
}
//-----------------------------------------------------------------------
void CDense::Read (std::istream & in) {
    m_matrix.resize(m_m);
    m_matrix.reserve(m_m);
    
    char character;
    double temp;
    in >> character; if (character != '[') throw WrongFormat();
    for (long i = 0; i < m_m; ++i) {
        for (long j = 0; j < m_n; j++)  
        {
            if (!( in >> temp)) throw WrongDimensions();
            m_matrix[i].push_back(temp);  
        }
        if (i != m_m - 1 && ( (! (in >> character) || character != ';')))
            throw WrongFormat();
    } 
}
//-----------------------------------------------------------------------
CMatrix * CDense::Clone () {
    // CDense temp (*this);
    return new CDense (*this);
}
//-----------------------------------------------------------------------
CDense::~CDense () {}
//=======================================================================