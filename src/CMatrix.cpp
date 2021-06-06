#include "../lib/CMatrix.h"

//============================================================================
CMatrix::CMatrix (int m, int n, bool is_matrix) : m_m (m), m_n (n), is_matrix (is_matrix) {}
//============================================================================
bool CMatrix::ColumnIsZero (int starting_row, int column) const {
    int cnt = 0;
    for (int j = starting_row; j < m_m; j++) {
        if (abs (GetValue (j, column)) <= 0.0001) cnt++;
        else break;
    }
    // cout << "number of the rows is " <<  << endl;
    if (cnt == m_m - starting_row) return true;
    return false;
}
//----------------------------------------------------------------------------
int CMatrix::ColumnNotZero (int starting_row, int column) const {
    for (int j = starting_row; j < m_m; j++) {
        if (abs(GetValue (j, column)) >= 0.00001) return j; 
    }
    return m_m+1;
}
//-----------------------------------------------------------------------
float CMatrix::RowsMinus (int starting_row, int column) {
    /*we have some matrix : (a b c ...)
                            (d e f ...)
                            (g h i ...)
                               ...
                            (  ...    )

    */
    float changes = 1;
    float value; //d
    float nasobek2;
    float odectene_cislo;
    float a = GetValue (starting_row, column);
     
    for (int i = starting_row + 1; i < m_m; i++) {
        nasobek2 = GetValue (i, column);
        for (int j = column; j < m_n; j++) {
            value = GetValue (i, j); //d
            odectene_cislo = GetValue (starting_row, j);
            SetValue (a * value - nasobek2 * odectene_cislo, i, j);
        }
        changes *= a;
    }

    return changes;
}
//----------------------------------------------------------------------------
bool Compare (float left, float right) {
    //compare if left value is equal to right one:
    // return left == right;
    return (fabs (left - right) <= 0.00001); 
}
//----------------------------------------------------------------------------
int CMatrix::FindMaxValInCol (int col, int starting_row) const {
    float max = GetValue (starting_row, col);
    float cur;
    int maxRow = starting_row;
    for (int i = starting_row; i < m_m; i++) {
        cur = GetValue (i, col);
        if (cur > max) maxRow = i;
    }
        
    return maxRow;
}
//----------------------------------------------------------------------------
void CMatrix::MultRow (int row, float factor) {
    float value;
    for (int j = 0; j < m_n; j++) {
        value = GetValue (row, j);
        SetValue (factor * value, row, j);
    }
}
//----------------------------------------------------------------------------
void CMatrix::MultAdd (int i, int j, float factor) {
    float value;
    for (int k = 0; k < m_n; k++){
        value = GetValue (j,k) * factor;
        SetValue (GetValue (i,k) + value, i,k);
    }
}
//----------------------------------------------------------------------------
bool CMatrix::leftHalfIsId () const {
    for (int i = 0; i < m_m; i++) { 
        for (int j = 0; j < m_m; j++) {
            if (i == j && !Compare (GetValue(i,j), 1)) return false;
            if (i != j && !Compare (GetValue(i,j), 0)) return false;
        }
    }
    return true;
}   
//----------------------------------------------------------------------------
bool CMatrix::IsSet (void) const {
    return m_m != 0 && m_n != 0;
}
//----------------------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CMatrix & matrix){
    //if it's float
    if (!matrix.is_matrix) {
        out << matrix.GetValue(0,0);
        return out;
    }

    for (int i = 0; i < matrix.m_m; i++ ) {
        for (int j = 0; j < matrix.m_n; j++) {
            if (!j) out << '(';
            out << std::setw(4) << std::fixed << std::setprecision(2) << matrix.GetValue (i,j);
            if (j != matrix.m_n - 1) out << ' ';
            else out << ')';
        }
        if (i != matrix.m_m - 1)
            out <<  std::endl;
    }
    return out;
}
//----------------------------------------------------------------------------
bool ShouldBeDense (const std::string & src) {
    // if ( number of not zero elements * (4 + 4 + 4) is less than number of all elements * 4)  
    // then it should be sparse else dense:
    int cntAll = 0, cntNotZero = 0;
    float v; char c;
    std::istringstream is (src);
    while ( is.good() ) {
        //if float can't be read, then stream has char as next value or eof
        if (!(is >> v)) {
            if (is.fail()) is.clear();
            if (is >> c) continue;
            else break;
        }
        cntAll++;
        if (v != 0) cntNotZero++;
    }
    std::cout << "cntNotZero is " << cntNotZero << ", cntAll is " << cntAll << std::endl;
    if (cntNotZero * 3 < cntAll) return false;
    return true;
}
//----------------------------------------------------------------------------
bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op) {
    int cntNotZero;
    if (op == '+' || op == '-') cntNotZero = CountSum (left, right, op);
    else if (op == '*') cntNotZero = CountMult        (left, right);
    if (cntNotZero * 3 < left.m_n * right.m_m) return false;
    return true;
}
//----------------------------------------------------------------------------
bool MergeShouldBeDense (const CMatrix & left, const CMatrix & right) {
    int cntNotZero = 0;
    for (int i = 0; i < left.m_m; i++) {
        for (int j = 0; j < left.m_n; j++) {
            if (left.GetValue(i,j) != 0) cntNotZero ++;
        }
    }
    for (int i = 0; i < right.m_m; i++) {
        for (int j = 0; j < right.m_n; j++) {
            if (right.GetValue(i,j) != 0) cntNotZero ++;
        }
    }
    if (cntNotZero * 3 < left.m_m*left.m_n + right.m_n*right.m_m) return false;
    return true;
}
//----------------------------------------------------------------------------
bool SplitShouldBeDense (const CMatrix & matrix, int m, int n, int c_m, int c_n) {
    int cntNotZero = 0;
    for (int i = c_m; i < m; i++)
        for (int j = c_n; j < n; j++)
        if (! Compare(matrix.GetValue (i,j), 0) ) cntNotZero ++;

    if (cntNotZero * 3 < m * n) return false;
    return true;
}
//----------------------------------------------------------------------------
int CountSum (const CMatrix & left, const CMatrix & right, char op) {
    int cnt = 0;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            if (op == '+' && left.GetValue(i,j) + right.GetValue(i,j) != 0) cnt++;
            else if (left.GetValue(i,j) - right.GetValue(i,j) != 0) cnt++;
        }
    }
    return cnt;
}
//----------------------------------------------------------------------------
int CountMult (const CMatrix & left, const CMatrix & right){
    int cnt = 0;
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < right.m_n; j++ ) {
            value = 0;
            for (int k = 0; k < left.m_n; k++) {
                value += left.GetValue(i,k) * right.GetValue(k,j);
            } 
            if (value != 0) cnt ++;
        }
    }
    return cnt;
}
//
void CMatrix::Read (std::istream & in) {
    float value;
    char c;
    if (! (in >> c) || c != '[') throw WrongFormat ("no opening brackets\n");
    for (int i = 0; i < m_m; i++) {
        for (int j = 0; j < m_n; j++) {
            if (!(in >> value)) throw WrongFormat ("error while reading matrix\n");
            SetValue (value, i, j);
            std::cout << "value is " << value << std::endl;
        }
        if ((i != m_m - 1) && (! (in >> c) || c != ';')) throw WrongFormat ("12error while reading matrix\n");
    } 
    if (! (in >> c) || c != ']') throw WrongFormat ("no opening brackets\n");
    std::cout << "read matrix is " << *this << std::endl;
}