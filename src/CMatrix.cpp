#include "../lib/CMatrix.h"

//============================================================================
CMatrix::CMatrix (int m, int n, bool is_matrix) : m_m (m), m_n (n), is_matrix (is_matrix) {}
//----------------------------------------------------------------------------
// CMatrix::CMatrix (float number) : m_m (1), m_n (1), is_matrix (false) {
//     CDense * t = new CDense (number);
//     this = t;
// }
//----------------------------------------------------------------------------
MPtr operator + (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetCoord(0,0) + right.GetCoord(0,0));
        }
        else throw WrongFormat("cannot sum by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
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
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetCoord(0,0) - right.GetCoord(0,0));
        }
        else throw WrongFormat("cannot subtract by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
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
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetCoord(0,0) * right.GetCoord(0,0));
        }
        else {
            MPtr temp;
            if ( dynamic_cast<const CDense *>(&right) ) 
                 temp = std::make_shared<CDense>  (right.m_m, right.m_n);
            else temp = std::make_shared<CSparse> (right.m_m, right.m_n);
            for (int i = 0; i < right.m_m; i ++) {
                for (int j = 0; j < right.m_n; j++) {
                    temp->SetCoord (left.GetCoord(0,0) * right.GetCoord(i,j), i , j);
                }
            }
            return temp;
        }
    }
    if (!right.is_matrix) throw WrongFormat ("cannot multiply by number from right\n");
    if (left.m_n != right.m_m) throw WrongDimensions();  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '*') )
        temp = std::make_shared<CDense>  (left.m_m, right.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, right.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < right.m_n; j++ ) {
            value = 0;
            for (int k = 0; k < left.m_n; k++) {
                value += left.GetCoord(i,k) * right.GetCoord(k,j);
            } 
            temp->SetCoord (value, i,j);
        }
    }
    return temp;
}
//----------------------------------------------------------------------------
int CMatrix::Rank () {
    auto gemmed = GEM ();
    int cntNotZero, h = 0;
    for (int i = m_m - 1; i >= 0; i--) {
        cntNotZero = 0;
        for (int j = m_n - 1; j >= 0; j--) {
            if (abs(gemmed->GetCoord (i,j)) <= 0.00001) cntNotZero++;
            else break;
        }
        if (cntNotZero == m_n) h++;
        else break;
    }   

    delete gemmed;
    return m_m - h;
}
//----------------------------------------------------------------------------
MPtr Merge (const CMatrix & left, const CMatrix & right) {
    if (left.m_m != right.m_m) throw WrongDimensions();
    MPtr temp;
    if (MergeShouldBeDense (left, right)) temp = std::make_shared<CDense> (left.m_m, left.m_n + right.m_n);
    else temp = std::make_shared<CSparse> (left.m_m, left.m_n + right.m_n);
    
    for (int i = 0; i < left.m_m; i++) {
        for (int j = 0; j < left.m_n; j++) {
            temp->SetCoord (left.GetCoord(i,j),i,j);
        }
    }
    for (int i = 0; i < right.m_m; i++) {
        for (int j = 0; j < right.m_n; j++) {
            temp->SetCoord (right.GetCoord(i,j),i,left.m_n + j);
        }
    }
    
    return temp;
}
//----------------------------------------------------------------------------
MPtr Split (const CMatrix & matrix, int m, int n, int c_m, int c_n) {
    if ( c_m - 1 + m > matrix.m_m || c_n - 1 + n > matrix.m_n) throw WrongDimensions();
    MPtr temp;
    if (SplitShouldBeDense (matrix, m, n,c_m, c_n)) 
         temp = std::make_shared<CDense> (m,n);
    else temp = std::make_shared<CSparse> (m,n);

    for (int i = 0 ; i < m; i++) {
        for (int j = 0; j < n; j++) {
            temp->SetCoord (matrix.GetCoord(i+c_m-1, j+c_n-1), i, j);
        }
    }

    return temp;
}
//----------------------------------------------------------------------------
CMatrix * CMatrix::GEM () {
    int l = 0, k = 0;
    bool over = false;
    auto temp = (this->Clone());
    float changes = 1;

    while (k < temp->m_n && l < temp->m_m ) {
        while (temp->ColumnIsZero (l, k)) {
            // temp->Print (cout);
            // cout << endl << endl;
            k++;
            if (k >= temp->m_n) {
                over = true;
                break;
            }
        }
        if (over) break;
        if (abs(temp->GetCoord (l, k)) <= 0.00001) {
            int row = temp->ColumnNotZero (l+1, k);
            if (row != m_m+1) {
                temp->SwapRows (row, l);
                changes *= (-1);
                // temp->Print (cout);
                // cout << endl << endl;
            }
        }
        if (abs(temp->GetCoord (l, k)) > 0.00001) {
            changes *= temp->RowsMinus (l, k);
            // temp->Print (cout);
            // cout << endl << endl;
            k++; l++;
        }
    }

    return temp;
}
//----------------------------------------------------------------------------
bool CMatrix::ColumnIsZero (int starting_row, int column) const {
    int cnt = 0;
    for (int j = starting_row; j < m_m; j++) {
        if (abs (GetCoord (j, column)) <= 0.0001) cnt++;
        else break;
    }
    // cout << "number of the rows is " <<  << endl;
    if (cnt == m_m - starting_row) return true;
    return false;
}
//----------------------------------------------------------------------------
int CMatrix::ColumnNotZero (int starting_row, int column) const {
    for (int j = starting_row; j < m_m; j++) {
        if (abs(GetCoord (j, column)) >= 0.00001) return j; 
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
    bool changed = false;
    // float changes = 
    float value; //d
    float nasobek2;
    float odectene_cislo;
    float a = GetCoord (starting_row, column);
    // if (starting_row + 1!= 
    for (int i = starting_row + 1; i < m_m; i++) {
        nasobek2 = GetCoord (i, column);
        for (int j = column; j < m_n; j++) {
            value = GetCoord (i, j); //d
            odectene_cislo = GetCoord (starting_row, j);
            SetCoord (a * value - nasobek2 * odectene_cislo, i, j);
            changed = true;
        }
    }

    return 1;
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
bool ProductShouldBeDense (const CMatrix & left, const CMatrix & right, char op) {
    int cntNotZero;
    if (op == '+' || op == '-') cntNotZero = CountSum (left, right, op);
    else if (op == '*') cntNotZero = CountProduct     (left, right);
    std::cout << "number of non-zero numbers is " << cntNotZero << std::endl;
    if (cntNotZero * 3 < left.m_n * right.m_m) return false;
    return true;
}
//----------------------------------------------------------------------------
bool MergeShouldBeDense (const CMatrix & left, const CMatrix & right) {
    int cntNotZero = 0;
    for (int i = 0; i < left.m_m; i++) {
        for (int j = 0; j < left.m_n; j++) {
            if (left.GetCoord(i,j) != 0) cntNotZero ++;
        }
    }
    for (int i = 0; i < right.m_m; i++) {
        for (int j = 0; j < right.m_n; j++) {
            if (right.GetCoord(i,j) != 0) cntNotZero ++;
        }
    }
    if (cntNotZero * 3 < left.m_m * left.m_n + right.m_n * right.m_m) return false;
    return true;
}
//----------------------------------------------------------------------------
bool SplitShouldBeDense (const CMatrix & matrix, int m, int n, int c_m, int c_n) {
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
    int cnt = 0;
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < right.m_n; j++ ) {
            value = 0;
            for (int k = 0; k < left.m_n; k++) {
                value += left.GetCoord(i,k) * right.GetCoord(k,j);
            } 
            if (value != 0) cnt ++;
        }
    }
    return cnt;
}