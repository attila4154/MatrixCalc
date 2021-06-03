#include <iostream>
#include <sstream>
#include "CMatrix.h"
#include "CDense.h"
#include "CSparse.h"
#include "../src/CMatrix.cpp"
#include "../src/CDense.cpp"
#include "../src/CSparse.cpp"
#include <string>
#include <set>

using namespace std;

int main () {
    shared_ptr<CMatrix> matrix = make_shared<CDense> ();
    cin >> *matrix;
    cout << matrix->ColumnIsZero (1,0);

    return 0;
}
