#include "hdr/CDense.h"


//=======================================================================
CDense::CDense               (int m, int n) : CMatrix (m,n, true) {
    m_matrix.resize(m);
    m_matrix.reserve(m);
    for (int i = 0; i < m_m; i++)
        m_matrix[i].resize(m_n);
}
//-----------------------------------------------------------------------
CDense::CDense               () : CMatrix (0,0, true) {
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
float CDense::GetValue (int m, int n) const {
    if (m >= m_m || n >= m_n) throw WrongDimensions ();
    return m_matrix [m][n];
}   
//-----------------------------------------------------------------------
void  CDense::SetValue (float value, int m, int n) {
    if (m > m_m || n > m_n) throw WrongDimensions();
    m_matrix[m][n] = value;
}
//-----------------------------------------------------------------------
void CDense::ReadRowValue (int row, float value) {
    if (row >= m_m) {
        m_m++;
        m_matrix.resize(m_m);
    }
    m_matrix[row].push_back(value);
}
//-----------------------------------------------------------------------
void CDense::SetCols (int columns) {
    if (columns < 0) 
        throw std::out_of_range ("number of cols can not be less than zero\n");
    m_n = columns;
}
//-----------------------------------------------------------------------
void CDense::SwapRows (int row1, int row2) {
    std::swap (m_matrix[row1], m_matrix[row2]);
}
//-----------------------------------------------------------------------
MPtr CDense::Transpose () const {
    std::shared_ptr<CDense> transposed = std::make_shared<CDense> (m_n, m_m);
    for (int i = 0; i < m_m; i++) {
        for (int j = 0; j < m_n; j++) {
            transposed->m_matrix[j][i] = m_matrix [i][j];
        }
    }
    return transposed;
}
//-----------------------------------------------------------------------
CMatrix * CDense::Clone () const {
    return new CDense (*this);
}
//-----------------------------------------------------------------------
CDense::~CDense () {}
//=======================================================================