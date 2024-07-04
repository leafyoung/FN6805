#include "tests.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;
using namespace std::placeholders;

void test_return() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  // get return = p / p_{prev}
  vector<double> v1{1.0978, 1.0959, 1.1003, 1.0952, 1.1008};
  vector<double> v2(v1.size() - 1);
  transform(next(v1.begin(), 1), v1.end(), v1.begin(), v2.begin(),
            [](auto a, auto b) { return a / b - 1; });
  for (auto v : v2) {
    cout << v << ", ";
  }
  cout << "\\n";
  transform(next(v1.begin(), 1), v1.end(), v1.begin(), v2.begin(),
            bind(minus<>(), bind(divides<>(), _1, _2), 1));

  for (auto v : v2) {
    cout << v << ", ";
  }
  cout << "\n";
}
