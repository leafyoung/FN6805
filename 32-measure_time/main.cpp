// https://github.com/leafyoung/FN6805/tree/main/32-measure_time

#include <chrono>
#include <iostream>
#include <vector>
using namespace std;
using namespace std::chrono;

// Change below without the use of reference
// volatile read: stops the optimizer from removing the copy
void test_value_copy(vector<int> x) { [[maybe_unused]] volatile int v{x[0]}; }

int main() {
  cout << "start\n";
  vector<int> xx(1'000'000ull, 1);
  auto start = steady_clock::now();
  for (int i = 0; i < 1'000; ++i) {
    test_value_copy(xx);
  }
  auto end = steady_clock::now();
  cout << "end\n";
  double time_taken = duration_cast<nanoseconds>(end - start).count() /
                      static_cast<double>(1e9);

  cout << time_taken << '\n';
}