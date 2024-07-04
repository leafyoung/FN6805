// https://replit.com/@YeKunlun/32-measuretime?v=1

#include <chrono>
#include <iostream>
#include <tuple>
#include <vector>
using namespace std;
using namespace std::chrono;

// Change below without the use of reference
void test_value_copy(vector<int> &x) { x.size(); }

int main() {
  cout << "start" << "\n";
  vector<int> xx(1'000'000ull, 1);
  auto start = high_resolution_clock::now();
  for (int i = 0; i < 1'000ull; ++i) {
    test_value_copy(xx);
  }
  auto end = high_resolution_clock::now();
  cout << "end" << "\n";
  double time_taken = duration_cast<nanoseconds>(end - start).count() /
                      static_cast<double>(1e9);

  cout << time_taken << "\n";
}