#include "../lib/CMatrix.h"

//============================================================================
CMatrix::CMatrix (int m, int n, bool is_matrix) : m_m (m), m_n (n), is_matrix (is_matrix) {}
//============================================================================
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
            if (abs(gemmed.first->GetCoord (i,j)) <= 0.00001) cntNotZero++;
            else break;
        }
        if (cntNotZero == m_n) h++;
        else break;
    }
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
            temp->SetCoord (matrix.GetCoord(i+c_m - 1, j+c_n - 1), i, j);
        }
    }

    return temp;
}
//----------------------------------------------------------------------------
std::pair<MPtr, float> CMatrix::GEM () {
    int l = 0, k = 0;
    bool over = false;
    auto temp = std::shared_ptr<CMatrix>(this->Clone());
    float changes = 1;

    while (k < temp->m_n && l < temp->m_m ) {
        while (temp->ColumnIsZero (l, k)) {
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
            }
        }
        if (abs(temp->GetCoord (l, k)) > 0.00001) {
            changes *= temp->RowsMinus (l, k);
            k++; l++;
        }
    }
    std::cout << "changes are " << changes << std::endl;
    std::pair<MPtr, float> gemmed (temp, changes);
    return gemmed;
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
    float changes = 1;
    float value; //d
    float nasobek2;
    float odectene_cislo;
    float a = GetCoord (starting_row, column);
     
    for (int i = starting_row + 1; i < m_m; i++) {
        nasobek2 = GetCoord (i, column);
        for (int j = column; j < m_n; j++) {
            value = GetCoord (i, j); //d
            odectene_cislo = GetCoord (starting_row, j);
            SetCoord (a * value - nasobek2 * odectene_cislo, i, j);
            changed = true;
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
MPtr CMatrix::Inverse() {
    if (m_n != m_m) throw WrongFormat ("matrix is not square\n");
    CMatrix * m = new CDense (m_m, 2 * m_n);
    for (int i = 0; i < m_m; i++ ) {
        for (int j = 0; j < m_n; j++) {
            m->SetCoord (GetCoord (i,j), i ,j);
        }
    }
    for (int i = 0; i < m_m; i++) {
        for (int j = m_n; j < m->m_n; j++) {
            if (i == j - m_n) m->SetCoord (1, i, j);
            else m->SetCoord (0, i, j);
        }
    }

    int cRow, cCol;
    int maxCount = 200;
    int count = 0;
    bool complete = false;

    std::cout << "from:\n" << *m << std::endl;

    while ((!complete) && (count < maxCount)) 
    {
        for (int diagIdx = 0; diagIdx < m->m_m; diagIdx++) 
        {
            cRow = diagIdx;
            cCol = diagIdx;

            int maxIdx = m->FindMaxValInCol (cCol, cRow);

            if (maxIdx != cRow) {
                std::cout << "swap rows: " << cRow << ',' << maxIdx << std::endl;
                m->SwapRows (cRow, maxIdx);
                std::cout << *m << std::endl;
            }
            //making current value is equal to one:
            if (!Compare (m->GetCoord (cRow, cCol), 1) )
            {
                float factor = 1.0 / m->GetCoord (cRow, cCol);
                std::cout << "multiply row " << cRow << " by factor " << factor << std::endl;
                m->MultRow (cRow, factor);
                std::cout << *m << std::endl;
            }

            // Column operations:
            for (int rowIndex = cRow + 1; rowIndex < m->m_m; rowIndex ++) 
            {
                if (!Compare (m->GetCoord (rowIndex, cCol), 0.0 ) ) 
                {
                    int rowOneIndex = cCol;

                    float curValue = m->GetCoord (rowIndex, cCol);

                    float rowOneValue = m->GetCoord (rowOneIndex, cCol);
                    
                    if (!Compare (rowOneValue, 0.0) ) {
                        float correctionFactor = - curValue / rowOneValue;
                        m->MultAdd (rowIndex, rowOneIndex, correctionFactor); 
                        std::cout << "multiply row " << rowOneIndex << " by " << correctionFactor <<
                        " and add to row " << rowIndex << std::endl;
                        std::cout << *m << std::endl;
                    }

                }

            } 
            //Row operations:

            for (int colIndex = cCol + 1; colIndex < m_n; colIndex++) {
                if (!Compare (m->GetCoord(cRow, colIndex), 0.0) ) {
                    int rowOneIndex = colIndex;

                    float curValue = m->GetCoord (cRow, colIndex);
                    float rowOneValue = m->GetCoord (rowOneIndex, colIndex);

                    if (!Compare (rowOneValue, 0.0) ) {
                        float correctionFactor = - curValue / rowOneValue;
                        m->MultAdd (cRow, rowOneIndex, correctionFactor);
                        std::cout << "multiply row " << rowOneIndex << " by " << correctionFactor <<
                        " and add to row " << cRow << std::endl;
                        std::cout << *m << std::endl;
                    }
                }  
            }
        }   
            if (m->leftHalfIsId() ) {
                complete = true;
                std::cout << *m << std::endl;
                std::cout << "matrix is inversed\n";
            }
        count++;
    }
    if (complete) {
        auto inversed = Split (*m, m_m, m_m, 1, m_n + 1 );
        delete m;
        return inversed;
    }

    delete m;
    return nullptr;
}
//----------------------------------------------------------------------------
int CMatrix::FindMaxValInCol (int col, int starting_row) const {
    float max = GetCoord (starting_row, col);
    float cur;
    int maxRow = starting_row;
    for (int i = starting_row; i < m_m; i++) {
        cur = GetCoord (i, col);
        if (cur > max) maxRow = i;
    }
        
    return maxRow;
}
//----------------------------------------------------------------------------
void CMatrix::MultRow (int row, float factor) {
    float value;
    for (int j = 0; j < m_n; j++) {
        value = GetCoord (row, j);
        SetCoord (factor * value, row, j);
    }
}
//----------------------------------------------------------------------------
void CMatrix::MultAdd (int i, int j, float factor) {
    float value;
    for (int k = 0; k < m_n; k++){
        value = GetCoord (j,k) * factor;
        SetCoord (GetCoord (i,k) + value, i,k);
    }
}
//----------------------------------------------------------------------------
bool CMatrix::leftHalfIsId () const {
    for (int i = 0; i < m_m; i++) { 
        for (int j = 0; j < m_m; j++) {
            if (i == j && !Compare (GetCoord(i,j), 1)) return false;
            if (i != j && !Compare (GetCoord(i,j), 0)) return false;
        }
    }
    return true;
}   
//----------------------------------------------------------------------------
float CMatrix::Determinant () {
    if (m_n != m_m) throw WrongFormat ("matrix is not square\n");
    auto gemmed = GEM ();
    float det = 1;
    for (int i = 0, j = 0; i < m_m; i++, j++)
        det *= gemmed.first->GetCoord(i,j);

    return det / gemmed.second;
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
    if (!matrix.is_matrix) {
        out << matrix.GetCoord(0,0);
        return out;
    }
    for (int i = 0; i < matrix.m_m; i++ ) {
        for (int j = 0; j < matrix.m_n; j++) {
            if (!j) out << '(';
            out << std::fixed << std::setprecision(2) << matrix.GetCoord (i,j);
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
    // for (int i = 
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
