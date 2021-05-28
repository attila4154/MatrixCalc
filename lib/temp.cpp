#include <iostream>
#include <sstream>
#include "CDense.h"
#include "../src/CDense.cpp"
#include <string>
#include <set>

#define SET std::set<std::string>

using namespace std;

int main () {

    SET s;
    s.emplace ("string");
    for (const auto & token : s) {
        cout << token << endl;
    }

    return 0;
}
