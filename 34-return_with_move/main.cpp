// https://replit.com/@YeKunlun/34-returnwithmove?v=1

#include <iostream>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;

// NRVO antipattern: writing `return std::move(vec)` DISABLES NRVO.
// The compiler no longer sees a plain named-variable return, so it must
// call the move constructor instead of constructing in-place.
// Always write `return vec;` — let the compiler apply NRVO for free.
vector<int> return_vector_move_antipattern(int n) {
  vector<int> vec(n, 7);
  cout << "in return_vector_move_antipattern()" << '\n';
  cout << &vec
       << " (vector object address inside function)\n"; // stack address of vec
  // BAD: forces a move, defeats NRVO
  // The compiler will warn: -Wpessimizing-move — that's the point!
  return std::move(vec); // NRVO suppressed; move ctor called instead
}

vector<int> return_vector(int n) {
  vector<int> vec(n, 5);
  cout << "in return_vector()" << '\n';
  cout << &vec[3] << ", " << vec[3] << '\n';
  return vec;
}

auto return_tuple() {
  auto t = make_tuple(3.3, "abc"s);
  cout << "in return_tuple()\n";
  cout << &t << '\n';
  return t;
}

struct Point {
  double x;
  double y;
};

Point return_point() {
  Point p{3, 5};
  cout << "in return_point()\n";
  cout << &p << '\n';
  return p;
}

struct PointComplex {
  double x;
  vector<int> y;
};

PointComplex return_point_complex() {
  PointComplex p{3, {5}};
  cout << "in return_point_complex()\n";
  cout << &p << '\n';
  return p;
}

int main() {
  // NRVO: compiler constructs directly in caller's space — no copy, no move
  auto x = return_vector(10);
  cout << "in main(): NRVO applied\n";
  cout << &x[3] << ", " << x[3] << '\n'; // same address as in function

  // Antipattern: return std::move() defeats NRVO, forces move constructor
  auto x_bad = return_vector_move_antipattern(10);
  cout << "in main(): NRVO suppressed by std::move (move ctor called)\n";
  cout
      << &x_bad
      << " (vector object address in main)\n"; // DIFFERENT from inside function
  // Note: &x_bad[3] (heap element) equals &vec[3] inside the function, even for
  // move, because move transfers the heap buffer pointer — NOT the object
  // itself. Only comparing vector OBJECT addresses (printed above) proves the
  // move constructor ran.

  auto t = return_tuple();
  cout << "in main(): NRVO applied (same address)\n";
  cout << &t << '\n';

  auto p = return_point();
  cout << "in main(): copied\n";
  cout << &p << '\n';

  auto pc = return_point_complex();
  cout << "in main(): NRVO applied (same address)\n";
  cout << &pc << '\n';

  {
    // move is copy for int
    int x = 3;
    int y = std::move(x);
    cout << x << ", " << y << '\n';
  }
  {
    // move is moving for vector
    vector<int> x1{3, 3};
    auto x2 = std::move(x1);
    cout << x2.size() << ", " << x1.size() << '\n';
  }
}