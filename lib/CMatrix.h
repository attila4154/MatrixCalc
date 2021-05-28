#include <iostream>

#define MPtr std::shared_ptr<CMatrix>

class CMatrix {
  public:
    CMatrix ();

    friend MPtr operator +(const CMatrix & left,
                           const CMatrix & right);
    friend MPtr operator -(const CMatrix & left,
                           const CMatrix & right);
    friend MPtr operator *(const CMatrix & left,
                           const CMatrix & right);
    virtual float GetCoord (int m, int n) = 0;
    void Print (std::ostream & out);


    friend std::istream & operator >>
     (std::istream & in, CMatrix & matrix);
    friend std::ostream & operator << 
     (std::ostream & out, const CMatrix & matrix);


  protected:
    int m_m;
    int m_n;
};