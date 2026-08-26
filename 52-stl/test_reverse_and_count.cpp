#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;
using namespace std::placeholders;

void test_reverse() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v{1, 3, 2, 5, 4};
  for (auto const val : v) {
    cout << val << ", ";
  }
  reverse(v.begin(), v.end());
  for (auto const val : v) {
    cout << val << ", ";
  }
  cout << endl;
}

void test_count() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v{1, 3, 2, 5, 4, 6, 3};

  // Look for number three
  auto three = 3;
  auto count_equal_3 = count(v.begin(), v.end(), three);
  // count_equal_3 is 1
  cout << count_equal_3 << endl;

  // Look for numbers which is larger than three, 3 ways
  auto is_above_3 = [](int v) { return v > 3; };
  auto count_above_3_1 = count_if(v.begin(), v.end(), is_above_3);
  auto count_above_3_2 =
      count_if(v.begin(), v.end(), [](int i) { return i > 3; });
  auto count_above_3_3 =
      count_if(v.begin(), v.end(), bind(greater<int>(), _1, 3));

  cout << count_above_3_1 << endl;
  cout << count_above_3_2 << endl;
  cout << count_above_3_3 << endl;
}
