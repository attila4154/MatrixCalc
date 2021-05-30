#include <iostream>
#include <sstream>
#include "CDense.h"
#include "../src/CDense.cpp"
#include "../src/CSparse.cpp"
#include "CSparse.h"
#include "CMatrix.h"
#include "../src/CMatrix.cpp"
#include <string>
#include <set>

using namespace std;

// bool ShouldBeDense (const std::string & src) {
//     // if ( number of not zero elements * (4 + 4 + 4) is less than number of all elements * 4)  
//     // then it should be sparse else dense:
//     int cntAll = 0, cntNotZero = 0;
//     float v; char c;
//     string str;
//     std::istringstream is (src);
//     cout << "string is " << src << endl;
//     while ( is.good() ) {
//         if (!(is >> v)) {
//             if (is.fail()) is.clear();
//             if (is >> c) continue;
//             else break;
//         }
//         cntAll++;
//         if (v != 0) cntNotZero++;
//         // if (is.eof() ) 
//     }
//     cout << "number of characters is " << cntAll << endl;
//     cout << "number of non-zero characters is " << cntNotZero << endl;
//     if (cntNotZero * 3 < cntAll) return true;
//     return false;
// }

int main () {
    CSparse matrix (4,4);
    matrix.Read (cin);
    matrix.Print (cout);

    return 0;
}
