#include <iostream>
#include <string>
#include <tuple>

using namespace std;

tuple<int, double, string> f() { return make_tuple(1, 2.3, "456"); }

// use auto
auto f2() { return make_tuple(1, 2.3, string("456")); }

// int
// string
// bool

int main() {
  auto [x, y, z] = f(); // from C++17
  cout << x << ", " << y << ", " << z << '\n';

  // above was a shortcut from tie
  {
    int x;
    double y;
    string z;
    tie(x, y, z) = f();
    cout << x << ", " << y << ", " << z << '\n';
  }

  auto t = make_tuple(42, string{"hi"}, true);
  auto a = get<0>(t);
  auto b = get<1>(t);
  auto c = get<2>(t);

  cout << a << ", " << b << ", " << c << '\n';

  return 0;
}
