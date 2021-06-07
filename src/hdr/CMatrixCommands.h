#pragma once
#include "CMatrix.h"
#include "CDense.h"
#include "CSparse.h"


/// friend class with main matrix operations
struct CCommands {
    static MPtr Transpose (const CMatrix & matrix);
    static MPtr Inverse   (const CMatrix & matrix);
    static int  Rank      (const CMatrix & matrix);
    /**
     * splits *matrix* with _m_ x _n_ size from row _c_m_ and column _c_n_
     */
    static MPtr Split     (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    static MPtr Merge     (const CMatrix & left, const CMatrix & right);
    /**
     * set _matrix_ to row echelon form 
     * @param[out] MPtr: pointer to gemmed matrix 
     * @param[out] float: changes that were made during GEM: (used for finding determinant)\n
     * * -1 for each 2 swapped rows
     * * f for each row multiplied by f
     */
    static std::pair<MPtr, float> GEM     (const CMatrix & matrix);
    static float Determinant (const CMatrix & matrix);
};