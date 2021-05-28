#pragma once

#include "../lib/CSparse.h"

class CSparse : public CMatrix {
public:
    CSparse              ();
    CSparse              (int m, int n);
    CSparse              (const std::string & src);
    CMatrix & operator + (const CMatrix & other) const;
    CMatrix & operator - (const CMatrix & other) const;
    CMatrix & operator * (const CMatrix & other) const;
    CMatrix & operator * (float alpha);
    CMatrix & Split      (int m, int n);
    CMatrix & Merge      (const CMatrix & other) const;
    CMatrix & Transpose  ();
    CMatrix & Inverse    ();
    CMatrix & GEM       ();
    int       Rank () const;
    float     Determinant () const;


    friend void GEM         (const CMatrix & matrix);
//    friend void GEM_comment (const CMatrix & matrix);
    void Print(std::ostream & out) const;
    virtual ~CSparse();
//-----------------------------------------------------------------
  private:
    //neco tady urcite bude...
};