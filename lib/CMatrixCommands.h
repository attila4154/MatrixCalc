#pragma once
#include "CMatrix.h"
#include "CDense.h"
#include "CSparse.h"



struct CCommands {


    static MPtr Transpose (const CMatrix & matrix);
    static MPtr Inverse (const CMatrix & matrix);
    static std::pair<MPtr, float> GEM     (const CMatrix & matrix);
    static int  Rank    (const CMatrix & matrix);
    static MPtr Split   (const CMatrix & matrix, int m, int n, int c_m, int c_n);
    static MPtr Merge   (const CMatrix & left, const CMatrix & right);
    static float Determinant (const CMatrix & matrix);


  private:
    // static bool Compare (float left, float right);
};