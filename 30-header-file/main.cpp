// https://replit.com/@YeKunlun/30-header-file?v=1

// This example is intentional to be un-compilable because we have included
// function definitions in a_func.h and b_func.h. It causes compiler error of
// re-definition.

#include <iostream>
using namespace std;

//
#include "a_func.h"
#include "b_func.h"

// Even we didn't include test_overload.h and c_func.h (if they exist), linker
// would find us the definition to build the program.

// Because we include the declaration of the functions.

void test_overload();
int c_func();

int main() {
  test_overload();
  cout << "a_func() + b_func(): " << a_func() + b_func() << "\n";
  cout << "c_func(): " << c_func() << '\n';
}