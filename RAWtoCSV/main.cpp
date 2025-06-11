/*

main.cpp
    LTspice 24.1.6
    LTspice_M ver. 0.0.4

*/


#include <iostream>
#include "LTspice_M.hpp"

int main() {
    std::string rawFileName;
    std::cout << ".raw file name : ";
    std::cin >> rawFileName;

    std::string csvFileName;
    std::cout << ".csv file name : ";
    std::cin >> csvFileName;

    LTspice_M obj;
    obj.rawToCsv(rawFileName, csvFileName);

    return 0;
}

