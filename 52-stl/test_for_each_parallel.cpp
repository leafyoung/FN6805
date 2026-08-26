#include "tests.h"
#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

using namespace std;

void test_for_each_parallel() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  std::vector<double> v{1, 2, 3, 4, 5};
  for (const auto &x : v) {
    cout << x << ", ";
  }
  cout << '\n';
  std::for_each(std::execution::par, v.begin(), v.end(),
                [](double &x) { ++x; });

  for (const auto &x : v) {
    cout << x << ", ";
  }
  cout << '\n';
  std::for_each(std::execution::par, v.begin(), v.end(),
                [](const auto &x) { cout << x << ", "; });
}