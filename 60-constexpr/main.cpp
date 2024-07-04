// https://replit.com/@YeKunlun/60-constexpr?v=1

#include <cmath>
#include <iostream>
#include <string>


constexpr auto sum(int x, int y, int z) { return x + y + z; }

constexpr int factorial(const int n) {
  return n <= 1 ? 1 : (n * factorial(n - 1));
}

int main() {
  constexpr auto z2 = 4;
  constexpr float x = 42.0 + z2;
  std::cout << x << "\n";

  constexpr auto value = sum(3, 4, 5);
  std::cout << value << "\n";

  constexpr auto factorial_10 = factorial(10);
  std::cout << factorial_10 << "\n";
}