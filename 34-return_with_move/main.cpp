// https://replit.com/@YeKunlun/34-returnwithmove?v=1

#include <iostream>
#include <tuple>
#include <vector>
using namespace std;

vector<int> return_vector(int n) {
  vector<int> vec(n, 5);
  cout << "in return_vector()\n";
  cout << &vec[3] << ", " << vec[3] << "\n";
  return vec;
}

auto return_tuple() {
  auto t = make_tuple(3.3, "abc"s);
  cout << "in return_tuple()\n";
  cout << &t << "\n";
  return t;
}

struct Point {
  double x;
  double y;
};

Point return_point() {
  Point p{3, 5};
  cout << "in return_point()\n";
  cout << &p << "\n";
  return p;
}

struct PointComplex {
  double x;
  vector<int> y;
};

PointComplex return_point_complex() {
  PointComplex p{3, {5}};
  cout << "in return_point_complex()\n";
  cout << &p << "\n";
  return p;
}

int main() {
  auto x = return_vector(10);
  cout << "in main(): moved\n";
  cout << &x[3] << ", " << x[3] << "\n"; // same address as in function

  auto t = return_tuple();
  cout << "in main(): moved\n";
  cout << &t << "\n";

  auto p = return_point();
  cout << "in main(): copied\n";
  cout << &p << "\n";

  auto pc = return_point_complex();
  cout << "in main(): moved\n";
  cout << &pc << "\n";

  {
    int x = 3;
    int y = move(x);
    cout << x << ", " << y << "\n";
    x += 1;
    y -= 1;
    cout << x << ", " << y << "\n";
  }
}