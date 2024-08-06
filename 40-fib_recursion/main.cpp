// https://replit.com/@YeKunlun/40-fibrecursion?v=1

#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
using namespace std::chrono;
using namespace std;
using namespace std::placeholders;

constexpr long long factorial(int n) {
  if (n == 0)
    return 1;

  return n * factorial(n - 1);
}

unsigned long long fib_linear(int n) {
  vector<unsigned long long> fibonacci(n + 1, 1);
  for (int i = 2; i <= n; ++i) {
    fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];
  }
  return fibonacci.back();
}

long long fib_recursive(int n) {
  if (n <= 1)
    return 1;
  return fib_recursive(n - 1) + fib_recursive(n - 2);
  // return (n <= 1) ? n : fib(n - 1) + fib(n - 2);
}

unsigned long long fib_recur_mem(int n) {
  const int NOT_INCLUDE = -1;
  vector<unsigned long long> temp(n + 1, NOT_INCLUDE);

  // need a type, cannot use auto
  function<unsigned long long(int)> fib_internal = [&temp,
                                                    &fib_internal](int n) {
    if (n <= 1)
      return 1ull;
    if (temp[n] != NOT_INCLUDE)
      return temp[n];
    temp[n] = fib_internal(n - 1) + fib_internal(n - 2);
    return temp[n];
  };

  return fib_internal(n);
}

void test_fib(function<int(int)> fib_fun, int n) {
  auto start = high_resolution_clock::now();
  auto i1 = fib_fun(n);
  auto end = high_resolution_clock::now();
  auto d1 = duration_cast<nanoseconds>(end - start).count();
  cout << i1 << ":(" << d1 << ") ns" << "\n";
}

int main() {
  {
    // warming up - 1st round
    test_fib(fib_linear, 3);
    test_fib(fib_recursive, 3);
    test_fib(fib_recur_mem, 3);

    // subsequent run is faster
    {
      test_fib(fib_linear, 3);
      test_fib(fib_recursive, 3);
      test_fib(fib_recur_mem, 3);
    }

    test_fib(fib_linear, 10);
    test_fib(fib_recursive, 10);
    test_fib(fib_recur_mem, 10);

    test_fib(fib_linear, 20);
    test_fib(fib_recursive, 20);
    test_fib(fib_recur_mem, 20);

    test_fib(fib_linear, 30);
    test_fib(fib_recursive, 30);
    test_fib(fib_recur_mem, 30);

    test_fib(fib_linear, 40);
    test_fib(fib_recursive, 40);
    test_fib(fib_recur_mem, 40);

    test_fib(fib_linear, 45);
    test_fib(fib_recursive, 45);
    test_fib(fib_recur_mem, 45);

    // overflow
    // test_fib(fib_linear, 50);
    // test_fib(fib_recursive, 50);
    // test_fib(fib_recur_mem, 50);
  }
}

void test_3072() {
  int x{3072};
  while (x % 2 == 0) {
    x /= 2;
    cout << x << "\n";
  }
  cout << x << "\n";
}
