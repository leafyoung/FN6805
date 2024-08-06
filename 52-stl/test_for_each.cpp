#include "tests.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

using namespace std;

void test_for_each() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> v_rand_d(20);
  iota(v_rand_d.begin(), v_rand_d.end(), 3);
  for (auto v : v_rand_d) {
    cout << v << ", ";
  }
  cout << "\n";

  int count_even = 0;
  for_each(v_rand_d.begin(), v_rand_d.end(),
           [&count_even](const int &x) { count_even += x % 2 == 0; });
  cout << "count_even: " << count_even << "\n";

  for_each(v_rand_d.begin(), v_rand_d.end(), [](auto x) { cout << x << ", "; });
  cout << "\n";

  for_each(v_rand_d.begin(), v_rand_d.end(), [](auto &x) {
    ++x;
    x = x * 2;
  });
  cout << "\n";
  int sum = 0;
  for_each(v_rand_d.begin(), prev(v_rand_d.end(), 3),
           [&sum](auto x) { sum += x; });
  cout << sum << "\n";

  for (auto v : v_rand_d) {
    sum += v;
  }

  for (auto x : v_rand_d) {
    cout << x << ", ";
  }
  cout << "\n";

  transform(v_rand_d.begin(), v_rand_d.end(), v_rand_d.begin(),
            [](int x) { return x * 2; });
  for (auto v : v_rand_d) {
    cout << v << ", ";
  }
  cout << "\n";

  transform(v_rand_d.begin(), v_rand_d.end(), v_rand_d.begin(),
            v_rand_d.begin(), [](int a, int b) { return a + b; });
  for (auto v : v_rand_d) {
    cout << v << ", ";
  }
  cout << "\n";

  sort(v_rand_d.begin(), v_rand_d.end(), [](int a, int b) { return a > b; });
  for (auto v : v_rand_d) {
    cout << v << ", ";
  }
  cout << "\n";

  // iota(next(v_rand_d.begin(), 4), prev(v_rand_d.end(),4), -3);
  iota(next(v_rand_d.begin(), 3), next(v_rand_d.begin(), 5), -3);
  for (auto v : v_rand_d) {
    cout << v << ", ";
  }
  cout << "\n";

  cout << "test_for_each\n";
  vector<int> v{2, 3, 1};
  sort(v.begin(), v.end());
  for_each(v.begin(), v.end(), [](int x) { cout << x << " "; });
  std::cout << "\n";
  // This results in infinite loop
  // for_each(v.end(), v.begin(), [](int x) { cout << x << " "; });
  // use rbegin()/rend()
  for_each(v.rbegin(), v.rend(), [](int x) { cout << x << " "; });
  std::cout << "\n";

  struct Option {
    double strike;
  };

  vector<Option> v_opt{Option{110.0}, Option{100.0}, Option{95.0}};
  sort(v_opt.begin(), v_opt.end(),
       [](const Option &a, const Option &b) { return a.strike < b.strike; });
  cout << v_opt.front().strike << "\n";
  cout << v_opt.back().strike << "\n";

  vector<tuple<string, string>> phone_numbers{make_tuple("88888888", "65"),
                                              make_tuple("11111111", "65"),
                                              make_tuple("12345678", "60")};

  sort(phone_numbers.begin(), phone_numbers.end());
  for_each(phone_numbers.begin(), phone_numbers.end(),
           [](auto v) { cout << get<0>(v) << " " << get<1>(v) << "\n"; });
  cout << "\n";

  sort(phone_numbers.begin(), phone_numbers.end(), [](auto a, auto b) {
    if (get<1>(a) == get<1>(b)) {
      return get<0>(a) < get<0>(b);
    } else {
      return get<1>(a) < get<1>(b);
    }
  });
  for_each(phone_numbers.begin(), phone_numbers.end(),
           [](auto v) { cout << get<0>(v) << " " << get<1>(v) << "\n"; });
  cout << "\n";
}
