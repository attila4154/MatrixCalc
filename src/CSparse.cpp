#include "../lib/CSparse.h"

//===========================================
CSparse::CSparse () : CMatrix (0,0) {
}
//-------------------------------------------
CSparse::CSparse (int m, int n) : CMatrix (m, n) {}
//-------------------------------------------
CSparse::CSparse (const CSparse & other) : CMatrix (other.m_m, other.m_n) {
    m_matrix = other.m_matrix; 
}
//-------------------------------------------
CMatrix * CSparse::Clone () {
    return new CSparse (*this);
}
//-------------------------------------------

//-------------------------------------------
float CSparse::GetCoord (int m, int n) const {
    /* check if sparse matrix contains elements under coords (m,n),
       if no, return 0, else return value (m,n); 
    */
    if (m > m_m || n > m_n) throw WrongDimensions ();
    for (int i = 0; i < m_matrix.size(); i++) {
        if (m_matrix.find (std::pair<int,int>(m,n)) != m_matrix.end() )
            return m_matrix.find (std::pair<int,int>(m,n))->second;
   }
   return 0;
}
//-------------------------------------------
void  CSparse::SetCoord (float value, int m, int n) {
    if (m > m_m || n > m_n) throw WrongDimensions ();
    if (value == 0) return;
    m_matrix.emplace (std::pair<int,int>(m, n), value);
}
//-------------------------------------------
void CSparse::Transpose () {
    std::map<std::pair<int,int>, float> transposed;

    for (auto & v : m_matrix) {
        transposed.emplace (std::pair<int,int> (v.first.second, v.first.first), v.second);
    }
    m_matrix = transposed;
    int temp = m_n;
    m_n = m_m;
    m_m = temp; 
}
//-------------------------------------------
void CSparse::Print (std::ostream & out) const {
    for (int i = 0; i < m_m; i++ ) {
        for (int j = 0; j < m_n; j++) {
            if (!j) out << '(';
            out << GetCoord (i,j);
            if (j != m_n - 1) out << ' ';
            else out << ')';
        }
        if (i != m_m - 1)
            out <<  std::endl;
    }
    out << "(sparse)";
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
            SetCoord (value, i, j);  
        }
        if (i != m_m - 1 && ( (! (in >> character) || character != ';')))
            throw WrongFormat();
    } 
}
//-------------------------------------------
CSparse::~CSparse () {}
//-------------------------------------------
