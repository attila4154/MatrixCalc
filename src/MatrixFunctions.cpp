#include "hdr/MatrixFunctions.h"

//----------------------------------------------------------------------
void ReadMatrix (std::istream & in, MPtr & matrix) {
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
    if (cntNotZero * 3 < cntAll) matrix = std::make_shared<CSparse> (m,n); 
    else matrix = std::make_shared<CDense> (m,n);
}
//----------------------------------------------------------------------
void ReadCoord (std::istream & in, int & m, int & n) { 
    //- format is '[ m, n ]
    char c;
    try {
        if (!(in >> c) || c != '[') throw ("");
        if (!(in >> m) || m < 0)    throw ("");
        if (!(in >> c) || c != ',') throw ("");
        if (!(in >> n) || n < 0)    throw ("");
        if (!(in >> c) || c != ']') throw ("");
    } catch (...) {
        throw WrongFormat ("could not read coordinates\n");
    }
}
//----------------------------------------------------------------------
void ReadSize (std::istream & in, int & m, int & n) {
    // size has format '( m , n )'
    char c;
    try {
        in >> c; if (c != '(') throw ("");
        in >> m;  
        in >> c; if (c != ',') throw ("");
        in >> n;
        in >> c; if (c != ')') throw ("");
    } catch (...) {
        throw WrongFormat ("could not read matrix size\n");
    }
}
//----------------------------------------------------------------------