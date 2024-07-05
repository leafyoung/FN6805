// https://replit.com/@YeKunlun/73-cachelocality

#include <array>
#include <chrono>
#include <iostream>
#include <random>
using namespace std::chrono;
using std::array;
using std::cout;

std::seed_seq seq{12123, 921312};
std::mt19937 gen(seq);
std::uniform_real_distribution<> dist(0.0, 10000.0);

template <unsigned int N> auto gen_arr() {
  array<array<double, N>, N> A;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      A[i][j] = dist(gen);
    }
  }
  return A;
}

template <unsigned int N>
void mult_a_b(array<array<double, N>, N> A, array<array<double, N>, N> B,
              array<array<double, N>, N> C) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int sum = 0;
      for (int k = 0; k < N; k++)
        sum += A[i][k] * B[k][j]; // row × column
      C[i][j] = sum;
    }
  }
}

template <unsigned int N>
void mult_a_bt(array<array<double, N>, N> A, array<array<double, N>, N> B,
               array<array<double, N>, N> C) {

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      int sum = 0;
      for (int k = 0; k < N; k++)
        sum += A[i][k] * B[j][k]; // row × row
      C[i][j] = sum;
    }
  }
}

int main() { // Test with 1024, 512
  auto A{gen_arr<1024>()};
  auto B{gen_arr<1024>()};
  auto C{gen_arr<1024>()};
  const auto iteration = 50'000'000'000llu;
  auto average = [](auto duration) {
    return static_cast<float>(duration.count()) / static_cast<float>(iteration);
  };
  for (int k = 0; k < 1; ++k) {
    {
      auto start = high_resolution_clock::now();
      for (unsigned long long i = 0; i < iteration; ++i)
        mult_a_b<1024>(A, B, C);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      cout << average(duration) << "\n";
    }
    {
      auto start = high_resolution_clock::now();
      for (unsigned long long i = 0; i < iteration; ++i)
        mult_a_bt<1024>(A, B, C);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      cout << average(duration) << " << \n";
    }
  }
}