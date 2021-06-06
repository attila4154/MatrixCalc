#include "../lib/CMatrixCommands.h"

//=============================================================
// bool CCommands::Compare (float left, float right) {
//     //compare if left value is equal to right one:
//     // return left == right;
//     return (fabs (left - right) <= 0.00001); 
// }
//-------------------------------------------------------------
MPtr CCommands::Inverse (const CMatrix & matrix){
    if (matrix.m_n != matrix.m_m) throw WrongFormat ("matrix is not square\n");
    CMatrix * m = new CDense (matrix.m_m, 2 * matrix.m_n);
    for (int i = 0; i < matrix.m_m; i++ ) {
        for (int j = 0; j < matrix.m_n; j++) {
            m->SetValue (matrix.GetValue (i,j), i ,j);
        }
    }
    for (int i = 0; i < matrix.m_m; i++) {
        for (int j = matrix.m_n; j < m->m_n; j++) {
            if (i == j - matrix.m_n) m->SetValue (1, i, j);
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
            //making current value equal to one:
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

            for (int colIndex = cCol + 1; colIndex < matrix.m_n; colIndex++) {
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
        auto inversed = Split (*m, matrix.m_m, matrix.m_m, 1, matrix.m_n + 1 );
        delete m;
        return inversed;
    }

    delete m;
    return nullptr;
}
//-------------------------------------------------------------
MPtr CCommands::Transpose (const CMatrix & matrix) {
    auto temp = matrix.Transpose();
    return temp;
}
//-------------------------------------------------------------
std::pair<MPtr, float> CCommands::GEM     (const CMatrix & matrix){
    int l = 0, k = 0;
    bool over = false;
    auto temp = std::shared_ptr<CMatrix>(matrix.Clone());
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
        if (fabs(temp->GetValue (l, k)) <= 0.00001) {
            int row = temp->ColumnNotZero (l+1, k);
            if (row != matrix.m_m + 1) {
                temp->SwapRows (row, l);
                changes *= (-1);
            }
        }
        if (fabs(temp->GetValue (l, k)) > 0.00001) {
            changes *= temp->RowsMinus (l, k);
            k++; l++;
        }
    }
    // std::cout << "changes are " << changes << std::endl;
    std::pair<MPtr, float> gemmed (temp, changes);
    return gemmed;
}
//-------------------------------------------------------------
int  CCommands::Rank (const CMatrix & matrix){
    auto gemmed = GEM (matrix);
    int cntNotZero, h = 0;
    for (int i = matrix.m_m - 1; i >= 0; i--) {
        cntNotZero = 0;
        for (int j = matrix.m_n - 1; j >= 0; j--) {
            if (abs(gemmed.first->GetValue (i,j)) <= 0.00001) cntNotZero++;
            else break;
        }
        if (cntNotZero == matrix.m_n) h++;
        else break;
    }
    return matrix.m_m - h;
}
// //-------------------------------------------------------------
MPtr CCommands::Split   (const CMatrix & matrix, int m, int n, int c_m, int c_n){
    if (c_m - 1 + m > matrix.m_m || c_n - 1 + n > matrix.m_n) throw WrongDimensions();
    MPtr temp;
    if (SplitShouldBeDense (matrix, m, n, c_m, c_n)) 
         temp = std::make_shared<CDense> (m,n);
    else temp = std::make_shared<CSparse> (m,n);

    for (int i = 0 ; i < m; i++) {
        for (int j = 0; j < n; j++) {
            temp->SetValue (matrix.GetValue(i+c_m - 1, j+c_n - 1), i, j);
        }
    }

    return temp;
}
// //-------------------------------------------------------------
MPtr CCommands::Merge   (const CMatrix & left, const CMatrix & right){
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
// //-------------------------------------------------------------
float CCommands::Determinant (const CMatrix & matrix){
    if (matrix.m_n != matrix.m_m) throw WrongFormat ("matrix is not square\n");
    auto gemmed = GEM (matrix);
    float det = 1;
    for (int i = 0, j = 0; i < matrix.m_m; i++, j++)
        det *= gemmed.first->GetValue(i,j);

    return det / gemmed.second;
}
//-------------------------------------------------------------