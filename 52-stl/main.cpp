// https://replit.com/@YeKunlun/52-stl?v=1

#include <iostream>

#include "tests.h"

using namespace std;

int main() {
  cout << boolalpha;

  test_vector();

  test_iota();
  test_for_each();

  test_binary_search();
  // throw logic_error("stop here");

  test_set1();
  test_set2();
  test_set3();
  test_map();

  test_find();

  test_search();
  test_return();
}