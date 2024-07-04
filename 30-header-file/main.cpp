// https://replit.com/@YeKunlun/30-header-file?v=1

#include <iostream>
using namespace std;

#include "a_func.h"
#include "b_func.h"
#include "c_func.cpp"

int main() { cout << a_func() + b_func() << "\n"; }