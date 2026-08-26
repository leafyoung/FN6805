#include <iostream>
#include <string>
#include <tuple>
using namespace std;

tuple<int, double, std::string> f() { return std::make_tuple(1, 2.3, "456"); }

// use auto
auto f2() { return std::make_tuple(1, 2.3, std::string("456")); }

// int
// std::string
// bool

int main() {
  auto [x, y, z] = f(); // from C++17
  std::cout << x << ", " << y << ", " << z << std::endl;

  // above was a shortcut from tie
  {
    int x;
    double y;
    string z;
    tie(x, y, z) = f();
    std::cout << x << ", " << y << ", " << z << std::endl;
  }

  auto t = std::make_tuple(42, std::string{"hi"}, true);
  auto a = std::get<0>(t);
  auto b = std::get<1>(t);
  auto c = std::get<2>(t);

  std::cout << a << ", " << b << ", " << c << std::endl;

  return 0;
}
