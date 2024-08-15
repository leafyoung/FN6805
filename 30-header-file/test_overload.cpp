#include <iostream>
using std::cout;

void print(double x) { cout << "double:" << x << "\n"; };
void print(float x) { cout << "float:" << x << "\n"; };
void print(int x) { cout << "int:" << x << "\n"; };
void print(int x, bool b = false) { cout << "int and bool:" << x << "\n"; };

void test_overload() {
  cout << "test_overload():\n";
  print(1.0);  // good, call double
  print(1.0f); // good, call float
  // print(1);       // error
  print(1, true); // good
  cout << "\n";
}
