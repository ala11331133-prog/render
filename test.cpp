#include "test.h"
#include <vector>
#include <iostream>

int g_TestCount = 0;
int g_TestErrors = 0;

void PrintTestResults()
{
    std::cout << "\n";
    std::cout << "====================\n";
    std::cout << "Tests:  " << g_TestCount << "\n";
    std::cout << "Errors: " << g_TestErrors << "\n";
    std::cout << "====================\n";
}
