#include "tests.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::placeholders;

void test_find() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v{1, 2, 3, 4, 5, 3, 2, 1};
  auto it = v.begin();
  vector<int> idx;
  while (it != v.end()) {
    // it = find(it, v.end(), 3);
    // Or, more flexible with greater_equal
    it = find_if(it, v.end(), bind(greater_equal<int>(), _1, 3));
    // this becomes 3>=
    // it = find_if(it, v.end(), bind(greater_equal<int>(), 3, _1));
    cout << *it << "\n";
    if (it != v.end()) {
      idx.push_back(it - v.begin());
      it++;
    }
  }
  cout << "loc: ";
  for (auto v : idx) {
    cout << v << ", ";
  }
  cout << "\n";

  cout << count(v.begin(), v.end(), 3) << "\n";
  cout << bind(plus<int>(), _1, 3)(3) << "\n";
  cout << bind(equal_to<int>(), _1, 3)(3) << "\n";
  cout << bind(greater_equal<int>(), _1, 4)(3) << "\n";
  cout << bind(greater_equal<int>(), 4, _1)(3) << "\n";
}
