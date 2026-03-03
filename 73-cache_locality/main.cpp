// https://replit.com/@YeKunlun/73-cachelocality

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <random>

using namespace std::chrono;
using std::array;
using std::cout;
using std::fill;

std::seed_seq seq{12123, 921312};
std::mt19937 gen(seq);
std::uniform_real_distribution<> dist(0.0, 10000.0);

using ELEM_TYPE = double;

template <size_t N> auto gen_arr() {
  array<array<ELEM_TYPE, N>, N> A;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      A[i][j] = dist(gen);
    }
  }
  return A;
}

template <size_t N>
void mult_a_b(array<array<ELEM_TYPE, N>, N> A, array<array<ELEM_TYPE, N>, N> B,
              array<array<ELEM_TYPE, N>, N> C) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ELEM_TYPE sum = 0;
      for (size_t k = 0; k < N; ++k)
        sum += A[i][k] * B[k][j]; // row × column
      C[i][j] = sum;
    }
  }
}

template <size_t N>
void mult_a_bt(array<array<ELEM_TYPE, N>, N> A, array<array<ELEM_TYPE, N>, N> B,
               array<array<ELEM_TYPE, N>, N> C) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ELEM_TYPE sum = 0;
      for (size_t k = 0; k < N; ++k)
        sum += A[i][k] * B[j][k]; // row × row
      C[i][j] = sum;
    }
  }
}

const size_t MATRIX_SIZE = 512;
const auto ITERATION = 100'000llu;

using ELEM_TYPE2 = int;

void sum_cached() {
  array<ELEM_TYPE2, MATRIX_SIZE * MATRIX_SIZE> arr{0};
  ELEM_TYPE2 x = 0;
  fill(arr.begin(), arr.end(), x++);
  ELEM_TYPE2 sum = 0;
  for (size_t i = 0; i < MATRIX_SIZE; ++i) {
    for (size_t j = 0; j < MATRIX_SIZE; ++j) {
      sum += arr[i * MATRIX_SIZE + j];
    }
  }
  (void)sum; // prevent optimisation from eliding the loop
}

void sum_uncached() {
  array<ELEM_TYPE2, MATRIX_SIZE * MATRIX_SIZE> arr{0};
  ELEM_TYPE2 x = 0;
  fill(arr.begin(), arr.end(), x++);
  ELEM_TYPE2 sum = 0;
  for (size_t i = 0; i < MATRIX_SIZE; ++i) {
    for (size_t j = 0; j < MATRIX_SIZE; ++j) {
      sum += arr[i + MATRIX_SIZE * j];
    }
  }
  (void)sum; // prevent optimisation from eliding the loop
}

int main() { // Test with 1024, 512
  auto A{gen_arr<MATRIX_SIZE>()};
  auto B{gen_arr<MATRIX_SIZE>()};
  auto C{gen_arr<MATRIX_SIZE>()};

  auto average = [](auto duration) {
    return static_cast<float>(duration.count()) / static_cast<float>(ITERATION);
  };
  for (int k = 0; k < 1; ++k) {
    {
      auto start = high_resolution_clock::now();
      for (unsigned long long i = 0; i < ITERATION; ++i)
        mult_a_b<MATRIX_SIZE>(A, B, C);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<nanoseconds>(stop - start);
      cout << average(duration) << '\n';
    }
    {
      auto start = high_resolution_clock::now();
      for (unsigned long long i = 0; i < ITERATION; ++i)
        mult_a_bt<MATRIX_SIZE>(A, B, C);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<nanoseconds>(stop - start);
      cout << average(duration) << " << (faster)\n";
    }
  }
  {
    auto start = high_resolution_clock::now();
    sum_cached();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);
    cout << average(duration) << " (faster)\n";
  }
  {
    auto start = high_resolution_clock::now();
    sum_uncached();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);
    cout << average(duration) << '\n';
  }
}