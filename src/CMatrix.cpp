#pragma once
#include "../lib/CMatrix.h"

std::istream & operator >> (std::istream & in, CMatrix & matrix){

}

std::ostream & operator << (std::ostream & out, const CMatrix & matrix){
    CMatrix.Print(out);
    return out;
}