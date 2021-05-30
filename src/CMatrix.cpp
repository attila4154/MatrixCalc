#include "../lib/CMatrix.h"

//============================================================================
CMatrix::CMatrix (int m, int n) : m_m (m), m_n (n) {}
//----------------------------------------------------------------------------
MPtr operator + (const CMatrix & left, const CMatrix & right) {
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '+') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            value = left.GetCoord (i,j) + right.GetCoord (i,j);
            temp->SetCoord (value, i, j);
        }
    }
    return temp;
}
//----------------------------------------------------------------------------
MPtr operator - (const CMatrix & left, const CMatrix & right) {
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '-') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            value = left.GetCoord (i,j) - right.GetCoord (i,j);
            temp->SetCoord (value, i, j);
        }
    }
    return temp;
}
//----------------------------------------------------------------------------
MPtr operator * (const CMatrix & left, const CMatrix & right) {
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '*') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);

    return temp;
}
//----------------------------------------------------------------------------
bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op) {
    int cntNotZero;
    if (op == '+' || op == '-') cntNotZero = CountSum (left, right, op);
    else if (op == '*') cntNotZero = CountProduct     (left, right);
    cout << "number of non-zero numbers is " << cntNotZero << endl;
    if (cntNotZero * 3 < left.m_n * right.m_m) return false;
    return true;
}
//----------------------------------------------------------------------------
int CountSum (const CMatrix & left, const CMatrix & right, char op) {
    int cnt = 0;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            if (op == '+' && left.GetCoord(i,j) + right.GetCoord(i,j) != 0) cnt++;
            else if (left.GetCoord(i,j) - right.GetCoord(i,j) != 0) cnt++;
        }
    }
    return cnt;
}
//----------------------------------------------------------------------------
int CountProduct (const CMatrix & left, const CMatrix & right){
    return 0;
}
//----------------------------------------------------------------------------
std::istream & operator >> (std::istream & in, std::shared_ptr<CMatrix> & matrix){
    std::string str;
    getline (in, str, ']');
    if (in.bad() ) throw WrongDimensions ();
    str += '[';
    if ( ShouldBeDense (str) ) {
        matrix = std::make_shared<CDense> ();
    } else {
        matrix = std::make_shared<CSparse>();
        std::cout << "matrix is sparse" << std::endl;
    }
    std::istringstream is (str);
    if (!matrix->IsSet () ) matrix->ReadSize (str);
    matrix->Read (is);
    return in;
}
//----------------------------------------------------------------------------
bool ShouldBeDense (const std::string & src) {
    // if ( number of not zero elements * (4 + 4 + 4) is less than number of all elements * 4)  
    // then it should be sparse else dense:
    int cntAll = 0, cntNotZero = 0;
    float v; char c;
    std::istringstream is (src);
    while ( is.good() ) {
        if (!(is >> v)) {
            if (is.fail()) is.clear();
            if (is >> c) continue;
            else break;
        }
        cntAll++;
        if (v != 0) cntNotZero++;
    }
    // cout << "number of characters is " << cntAll << endl;
    // cout << "number of non-zero characters is " << cntNotZero << endl;
    if (cntNotZero * 3 < cntAll) return false;
    return true;
}
//----------------------------------------------------------------------------
void CMatrix::ReadSize (std::string & src) {
    std::istringstream is (src);
    int cnt_n = 0;
    int cnt_m = 1;
    float temp;
    char c;
    is >> c;
    //finding how many columns matrix has: 
    do {
        if ( ! (is >> temp) ) break;
        cnt_n++;
    } while ( !is.eof() );
    //finding how many rows it has:
    long unsigned int i = 0;
    do {
        if (src[i] == ';')
            cnt_m ++; 
        i++;
    } while (i != src.size() - 1);
    std::cout << "matrix has " << cnt_m << " rows and " << cnt_n << " columns\n";

    m_n = cnt_n;
    m_m = cnt_m;
}
//----------------------------------------------------------------------------
bool CMatrix::IsSet (void) const {
    return m_m != 0 && m_n != 0;
}
//----------------------------------------------------------------------------
std::ostream & operator << (std::ostream & out, const CMatrix & matrix){
    matrix.Print (out);
    return out;
}
//----------------------------------------------------------------------------