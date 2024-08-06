#include "tests.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

void test_iota() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  array<int, 7> seq;
  // vector<int> seq(7);
  iota(seq.rbegin(), seq.rend(), 0);
  for (auto v : seq) {
    cout << v << ", ";
  }
  cout << "\n";

  auto it = max_element(seq.begin(), seq.end());
  cout << *it << "\n";

  it = min_element(seq.begin(), seq.end());
  cout << *it << "\n";

  int cc = 0;
  generate(seq.begin(), next(seq.begin(), 10), [&cc]() {
    ++cc;
    return cc * cc;
  });

  for (auto v : seq) {
    cout << v << ", ";
  }
  cout << "\n";
  // throw logic_error("stopped here");
}
