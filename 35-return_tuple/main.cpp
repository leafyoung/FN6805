// https://github.com/leafyoung/FN6805/tree/main/35-return_tuple

#include <iostream>
#include <tuple>
#include <string>
#include <vector>
using namespace std;

tuple<int, double, string> get_tuple() {
  return make_tuple(1, 2.3, "456"s);
}
auto get_tuple2() {
  return make_tuple(vector<double>{3.7, 8.6}, 2.3, "456"s);
}

int main() {
  auto [x, y, z] = get_tuple();
  cout << x << ", " << y << ", " << z << '\n';

  {
    // Before, use std::tie to unpack a tuple
    int x; double y; string z;
    tie(x, y, z) = get_tuple();
  }

  // Or, use `get`.
  auto t = get_tuple();
  auto a = std::get<0>(t);
  auto b = std::get<1>(t);
  auto c = std::get<2>(t);
  cout << a << ", " << b << ", " << c << '\n';
}
