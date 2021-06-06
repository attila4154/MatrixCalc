#include "../lib/MatrixFunctions.h"

void ReadMatrix (std::istream & in, MPtr & matrix) {
    /* 
        function that reads matrix of given size and stores it as Dense or Sparse matrix 
        depending on how many 'empty characters' (0) it has                           
    */

    std::string str;
    char c;
    if (!(in >> c) || c != '[') throw WrongFormat ();
    str.push_back (c);
    do {
        in.read (&c, sizeof(char));
        str.push_back (c);
    } while (! (c == ']'  || in.eof() || in.bad()) );
    if (in.fail() ) throw WrongFormat ();

    SetMatrixSize (matrix, str);
    std::istringstream is (str);
    matrix->Read (is);
}
//----------------------------------------------------------------------
void SetMatrixSize (MPtr & matrix, const std::string & src) {
    /*  
        function that decides whether matrix should be dense or sparse
        and sets its size
    */
    std::istringstream is (src);
    int cntAll = 0, cntNotZero = 0, m = 1, n = 0;
    float v; char c;
    while ( is.good() ) {
        //if float can't be read, then stream has char as next value or eof
        if (!(is >> v)) {
            if (is.fail()) is.clear();
            if (is >> c) {
                if (c == ';') m++;
                continue;
            }
            else break;
        }
        cntAll++;
        // while reading first row, count how many columns
        if (m < 2) n++;
        if (v != 0) cntNotZero++;
    }
    std::cout << "cntNotZero is " << cntNotZero << ", cntAll is " << cntAll << std::endl;
    std::cout << "size is " << m << "," << n << std::endl;
    if (cntNotZero * 3 < cntAll) matrix = std::make_shared<CSparse> (m,n); 
    else matrix = std::make_shared<CDense> (m,n);
}
//----------------------------------------------------------------------
// void SetMatrixSize (MPtr & matrix, std::istream & in) {
//     int cntAll = 0, cntNotZero = 0, m = 1, n = 0;
//     float v; char c;
//     while ( in.good() ) {
//         //if float can't be read, then stream has char as next value or eof
//         if (!(in >> v)) {
//             if (in.fail()) is.clear();
//             if (in >> c) {
//                 if (c == ';') m++;
//                 continue;
//             }
//             else break;
//         }
//         cntAll++;
//         // while reading first row, count how many columns
//         if (m < 2) n++;
//         if (v != 0) cntNotZero++;
//     }
//     // std::cout << "cntNotZero is " << cntNotZero << ", cntAll is " << cntAll << std::endl;
//     // std::cout << "size is " << m << "," << n << std::endl;
//     if (cntNotZero * 3 < cntAll) matrix = std::make_shared<CSparse> (m,n); 
//     else matrix = std::make_shared<CDense> (m,n);
// }
//----------------------------------------------------------------------