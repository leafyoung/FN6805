// https://github.com/leafyoung/FN6805/tree/main/34-return_with_move

#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;

// NRVO antipattern: writing `return std::move(vec)` DISABLES NRVO.
// The compiler no longer sees a plain named-variable return, so it must
// call the move constructor instead of constructing in-place.
// Always write `return vec;` — let the compiler apply NRVO for free.
vector<int> return_vector(int n) {
  vector<int> vec(n, 5);
  cout << "\nin return_vector()" << '\n';
  cout << &vec[3] << ", " << vec[3] << '\n';
  // BAD: forces a move, defeats NRVO
  // The compiler will warn: -Wpessimizing-move — that's the point!
  // return std::move(vec); // NRVO suppressed; move ctor called instead
  return vec;
}

auto return_tuple() {
  auto t = make_tuple(3.3, "abc"s);
  cout << "\nin return_tuple()\n";
  cout << &t << '\n';
  return t;
}

struct Point {
  double x;
  double y;
};

Point return_point() {
  Point p{3, 5};
  cout << "\nin return_point()\n";
  cout << &p << '\n';
  return p;
}

struct PointComplex {
  double x;
  vector<int> y;
};

PointComplex return_point_complex() {
  PointComplex p{3, {5}};
  cout << "\nin return_point_complex()\n";
  cout << &p << '\n';
  return p;
}

int main() {
  // NRVO: compiler constructs directly in caller's space — no copy, no move
  auto x = return_vector(10);
  cout << "\nin main(): NRVO applied\n";
  cout << &x[3] << ", " << x[3] << '\n'; // same address as in function

  auto t = return_tuple();
  cout << "\nin main(): NRVO applied (same address)\n";
  cout << &t << '\n';

  auto p = return_point();
  cout << "\nin main(): copied\n";
  cout << &p << '\n';

  auto pc = return_point_complex();
  cout << "\nin main(): NRVO applied (same address)\n";
  cout << &pc << '\n';

  {
    cout << "\nmove is copy for int:\n";
    int x = 3;
    int y = std::move(x);
    cout << x << ", " << y << '\n';
  }
  {
    cout << "\nmove is **move** for vector:\n";
    vector<int> x1{3, 3};
    auto x2 = std::move(x1);
    cout << x2.size() << ", " << x1.size() << '\n';
  }
}