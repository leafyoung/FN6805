#include "tests.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

void test_binary_search() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v{1, 3, 2, 5, 4};

  // Cannot find
  cout << "unsorted\n";
  for (auto val : array<int, 5>{1, 2, 3, 4, 5}) {
    cout << val << ": " << binary_search(v.begin(), v.end(), val) << "\n";
  }

  // Can find
  cout << "sorted\n";
  sort(v.begin(), v.end());
  for (auto val : array<int, 5>{1, 2, 3, 4, 5}) {
    cout << val << ": " << binary_search(v.begin(), v.end(), val) << "\n";
  }

  cout << accumulate(v.begin(), v.end(), 0.0) / v.size() << "\n";
}