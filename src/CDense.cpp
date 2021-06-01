#pragma once

#include "../lib/CDense.h"

//=======================================================================
CDense::CDense               (int m, int n) : CMatrix (m,n) {
    m_matrix.resize(m);
    m_matrix.reserve(m);
    for (int i = 0; i < m_m; i++)
        m_matrix[i].resize(m_n);
}
//-----------------------------------------------------------------------
CDense::CDense               () : CMatrix (0,0) {
    // m_matrix.resize(m_m);
    // m_matrix.reserve(m_m);
}
//-----------------------------------------------------------------------
CDense::CDense (const CDense & other) : CMatrix (other.m_m, other.m_n) {
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
// CDense & CDense::operator = (const CDense & other) {
//     if (&other == this) return *this;
//     CDense temp (other);
//     std::swap (temp.m_m, m_m);
//     std::swap (temp.m_n, m_n);
//     std::swap (temp.m_matrix, m_matrix);
//     return *this;
// }
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
void CDense::Transpose () {
    std::vector<std::vector <float>> temp;
    temp.resize(m_n);
    for (int i = 0; i < m_n; i++) 
        temp[i].resize (m_m);

    for (int i = 0; i < m_m; i++) {
        for (int j = 0; j < m_n; j++) {
            temp[j][i] = m_matrix [i][j];
        }
    }
    m_matrix = temp;
    int tempN = m_n;
    m_n = m_m;
    m_m = tempN;
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
//-----------------------------------------------------------------------
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
// shared_ptr<CDense> operator * (float alpha, const CDense & matrix){
//     shared_ptr<CDense> temp = make_shared<CDense> (matrix.m_m, matrix.m_n);
//     for (int i = 0; i < temp.m_m; i++) {
//         temp.m_matrix[i].resize(temp.m_n);
//         temp.m_matrix[i].reserve(temp.m_n);
//     }
//     for (int i = 0; i < matrix.m_m; ++i) {
//         for (int j = 0; j < matrix.m_n; j++)  
//         {
//             temp.m_matrix[i][j] = matrix.m_matrix[i][j] * alpha;
//         }
//     } 

//     return temp;
// }

// shared_ptr<CDense> operator * (const CDense & left, const CDense & right) {
//     if (left.m_n != right.m_m) throw WrongFormat ();
//     shared_ptr<CDense> temp = make_shared <CDense> (left.m_m, right.m_n);
    

//     return temp;
// }
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