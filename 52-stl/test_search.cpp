#include "tests.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

void test_search() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v{1, 2, 3, 4, 5, 3, 2, 1};
  vector<int> u{3, 4, 5};
  auto it = search(v.begin(), v.end(), u.begin(), u.end());
  if (it != v.end()) {
    cout << *it << "\n";
    cout << (it - v.begin()) << "\n";
  } else {
    cout << "not found\n";
  }
}
