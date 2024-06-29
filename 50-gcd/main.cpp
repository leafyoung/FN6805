#include <iostream>
#include <memory>
#include <vector>
using namespace std;

int gcd(int u, int v) {
  if (u < 0)
    u = -u;
  if (v < 0)
    v = -v;
  int r;
  while (v != 0) {
    r = u % v;
    u = v;
    v = r;
  }
  return u;
}

int gcd2(int n1, int n2) {
  while (n1 != n2) {
    if (n1 > n2)
      n1 -= n2;
    else
      n2 -= n1;
  }
  return n1;
}
// using namespace std;

int main() {
  cout << "gcd\n";
  cout << gcd(10, 3) << "\n";
  cout << gcd(10, 2) << "\n";
  cout << gcd(9, 3) << "\n";
  cout << gcd(10, 5) << "\n";

  cout << "gcd2\n";
  cout << gcd2(10, 3) << "\n";
  cout << gcd2(10, 2) << "\n";
  cout << gcd2(9, 3) << "\n";
  cout << gcd2(10, 5) << "\n";
}