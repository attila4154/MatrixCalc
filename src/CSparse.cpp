#include "../lib/CSparse.h"

//===========================================
CSparse::CSparse () : CMatrix (0,0, true) {
}
//-------------------------------------------
CSparse::CSparse (int m, int n) : CMatrix (m, n, true) {}
//-------------------------------------------
CSparse::CSparse (const CSparse & other) : CMatrix (other.m_m, other.m_n, true) {
    m_matrix = other.m_matrix; 
}
//-------------------------------------------
CMatrix * CSparse::Clone () const {
    return new CSparse (*this);
}
//-------------------------------------------
float CSparse::GetValue (int m, int n) const {
    /* 
        check if sparse matrix contains elements with coords (m,n),
       if no, return 0, else return value (m,n); 
    */
    if (m >= m_m || n >= m_n) throw WrongDimensions ();
    for (long unsigned int i = 0; i < m_matrix.size(); i++) {
        if (m_matrix.find (std::pair<int,int>(m,n)) != m_matrix.end() )
            return m_matrix.find (std::pair<int,int>(m,n))->second;
   }
   return 0;
}
//-------------------------------------------
void  CSparse::SetValue (float value, int m, int n) {
    if (m > m_m || n > m_n) throw WrongDimensions ();
    if (value == 0) return;
    m_matrix.emplace (std::pair<int,int>(m, n), value);
}
//-------------------------------------------
void CSparse::SwapRows (int row1, int row2) {
    float value1, value2;
    for (int i = 0; i < m_n; i++) {
        value1 = GetValue (row1, i);
        value2 = GetValue (row2, i);
        m_matrix[std::pair<int,int> (row1,i)] = value2;
        m_matrix[std::pair<int,int> (row2,i)] = value1;
    }
}
//-----------------------------------------------------------------------
MPtr CSparse::Transpose () {
    std::shared_ptr<CSparse> transposed = std::make_shared<CSparse> (m_n, m_m);

    for (auto & v : m_matrix) {
        transposed->m_matrix.emplace (std::pair<int,int> (v.first.second, v.first.first), v.second);
    }

    return transposed;
}
//-------------------------------------------
void CSparse::Read  (std::istream & in ) {
    char character;
    double value;
    in >> character; if (character != '[') throw WrongFormat();
    for (long i = 0; i < m_m; ++i) {
        for (long j = 0; j < m_n; j++)  
        {
            if (!( in >> value)) throw WrongDimensions();
            SetValue (value, i, j);  
        }
        if (i != m_m - 1 && ( (! (in >> character) || character != ';')))
            throw WrongFormat();
    } 
}
//-------------------------------------------
CSparse::~CSparse () {}
//-------------------------------------------
