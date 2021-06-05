#include "../lib/CMatrix.h"

//============================================================================
CMatrix::CMatrix (int m, int n, bool is_matrix) : m_m (m), m_n (n), is_matrix (is_matrix) {}
//============================================================================
MPtr operator + (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetValue(0,0) + right.GetValue(0,0));
        }
        else throw WrongFormat("cannot sum by number\n");
    }
    if (!right.is_matrix) throw WrongFormat("cannot sum by number\n");
    if (left.m_m != right.m_m || left.m_n != right.m_n) throw WrongDimensions(left.m_m, left.m_n,
                                                                             right.m_m, right.m_n);  
    std::shared_ptr<CMatrix> temp;
    if ( ProductShouldBeDense (left, right, '+') )
        temp = std::make_shared<CDense>  (left.m_m, left.m_n);
    else 
        temp = std::make_shared<CSparse> (left.m_m, left.m_n);
    float value;
    for (int i = 0; i < left.m_m; i++ ){
        for (int j = 0; j < left.m_n; j++ ) {
            value = left.GetValue (i,j) + right.GetValue (i,j);
            temp->SetValue (value, i, j);
        }
    }
    return temp;
}
//----------------------------------------------------------------------------
MPtr operator - (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetValue(0,0) - right.GetValue(0,0));
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
            value = left.GetValue (i,j) - right.GetValue (i,j);
            temp->SetValue (value, i, j);
        }
    }
    return temp;
}
//----------------------------------------------------------------------------
MPtr operator * (const CMatrix & left, const CMatrix & right) {
    if (!left.is_matrix) {
        if (!right.is_matrix) {
            return std::make_shared<CDense> (left.GetValue(0,0) * right.GetValue(0,0));
        }
        else {
            MPtr temp;
            if ( dynamic_cast<const CDense *>(&right) ) 
                 temp = std::make_shared<CDense>  (right.m_m, right.m_n);
            else temp = std::make_shared<CSparse> (right.m_m, right.m_n);
            for (int i = 0; i < right.m_m; i ++) {
                for (int j = 0; j < right.m_n; j++) {
                    temp->SetValue (left.GetValue(0,0) * right.GetValue(i,j), i , j);
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
                value += left.GetValue(i,k) * right.GetValue(k,j);
            } 
            temp->SetValue (value, i,j);
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
            if (abs(gemmed.first->GetValue (i,j)) <= 0.00001) cntNotZero++;
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
            temp->SetValue (left.GetValue(i,j),i,j);
        }
    }
    for (int i = 0; i < right.m_m; i++) {
        for (int j = 0; j < right.m_n; j++) {
            temp->SetValue (right.GetValue(i,j),i,left.m_n + j);
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
            temp->SetValue (matrix.GetValue(i+c_m - 1, j+c_n - 1), i, j);
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
        if (abs(temp->GetValue (l, k)) <= 0.00001) {
            int row = temp->ColumnNotZero (l+1, k);
            if (row != m_m+1) {
                temp->SwapRows (row, l);
                changes *= (-1);
            }
        }
        if (abs(temp->GetValue (l, k)) > 0.00001) {
            changes *= temp->RowsMinus (l, k);
            k++; l++;
        }
    }
    // std::cout << "changes are " << changes << std::endl;
    std::pair<MPtr, float> gemmed (temp, changes);
    return gemmed;
}
//----------------------------------------------------------------------------
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
MPtr CMatrix::Inverse() {
    if (m_n != m_m) throw WrongFormat ("matrix is not square\n");
    CMatrix * m = new CDense (m_m, 2 * m_n);
    for (int i = 0; i < m_m; i++ ) {
        for (int j = 0; j < m_n; j++) {
            m->SetValue (GetValue (i,j), i ,j);
        }
    }
    for (int i = 0; i < m_m; i++) {
        for (int j = m_n; j < m->m_n; j++) {
            if (i == j - m_n) m->SetValue (1, i, j);
            else m->SetValue (0, i, j);
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
            if (!Compare (m->GetValue (cRow, cCol), 1) )
            {
                float factor = 1.0 / m->GetValue (cRow, cCol);
                std::cout << "multiply row " << cRow << " by factor " << factor << std::endl;
                m->MultRow (cRow, factor);
                std::cout << *m << std::endl;
            }

            // Column operations:
            for (int rowIndex = cRow + 1; rowIndex < m->m_m; rowIndex ++) 
            {
                if (!Compare (m->GetValue (rowIndex, cCol), 0.0 ) ) 
                {
                    int rowOneIndex = cCol;

                    float curValue = m->GetValue (rowIndex, cCol);

                    float rowOneValue = m->GetValue (rowOneIndex, cCol);
                    
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
                if (!Compare (m->GetValue(cRow, colIndex), 0.0) ) {
                    int rowOneIndex = colIndex;

                    float curValue = m->GetValue (cRow, colIndex);
                    float rowOneValue = m->GetValue (rowOneIndex, colIndex);

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
float CMatrix::Determinant () {
    if (m_n != m_m) throw WrongFormat ("matrix is not square\n");
    auto gemmed = GEM ();
    float det = 1;
    for (int i = 0, j = 0; i < m_m; i++, j++)
        det *= gemmed.first->GetValue(i,j);

    return det / gemmed.second;
}
//----------------------------------------------------------------------------
std::istream & operator >> (std::istream & in, std::shared_ptr<CMatrix> & matrix){
    /*
        matrix has format : '[ ... ; ... ... ; ... ]
    */
    std::string str;
    char c;
    do {
        in.read (&c, sizeof(char));
        str.push_back (c);
    } while (! (c == ']'  || in.eof() || in.bad()) );
    if (in.fail() ) throw WrongFormat ();
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
    m_m = 1;
    float temp;
    char c;
    is >> c;
    //finding how many columns matrix has: 
    do {
        if ( ! (is >> temp) ) break;
        m_n++;
    } while ( !is.eof() );
    //finding how many rows it has:
    long unsigned int i = 0;
    do {
        if (src[i] == ';')
            m_m ++; 
        i++;
    } while (i != src.size() - 1);
    // std::cout << "matrix has " << cnt_m << " rows and " << cnt_n << " columns\n";
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
    if (dynamic_cast <const CSparse*> (&matrix) )
    out << "(sparse)";
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
