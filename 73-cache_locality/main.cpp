// https://replit.com/@YeKunlun/73-cachelocality

// Cache locality demo: memory access order matters because the CPU loads data
// in 64-byte cache lines (8 doubles or 16 ints per line). Sequential (row-
// major) access reuses each loaded line; strided (column-major) access wastes
// most of every line fetched.

#include <array>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>

using namespace std::chrono;
using std::array;
using std::cout;

// --- Matrix multiply helpers -----------------------------------------------

std::seed_seq seq{90128, 43021, 89427};
std::mt19937_64 gen(seq);
std::uniform_real_distribution<> dist(0.0, 10000.0);

using ELEM_TYPE = double;

template <size_t N> auto gen_arr() {
  array<array<ELEM_TYPE, N>, N> A{};
  for (size_t i = 0; i < N; ++i)
    for (size_t j = 0; j < N; ++j)
      A[i][j] = dist(gen);
  return A;
}

// A * B: inner loop accesses B[k][j] with k varying -> column-wise, stride N
// Each B element is 512*8 = 4096 bytes away from the last -> poor cache reuse
template <size_t N>
void mult_col_wise(const array<array<ELEM_TYPE, N>, N> &A,
                   const array<array<ELEM_TYPE, N>, N> &B,
                   array<array<ELEM_TYPE, N>, N> &C) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ELEM_TYPE sum = 0;
      for (size_t k = 0; k < N; ++k)
        sum += A[i][k] * B[k][j]; // row x column
      C[i][j] = sum;
    }
  }
}

// A * B^T: inner loop accesses B[j][k] with k varying -> row-wise, stride 1
// Consecutive B elements are adjacent in memory -> good cache reuse
template <size_t N>
void mult_row_wise(const array<array<ELEM_TYPE, N>, N> &A,
                   const array<array<ELEM_TYPE, N>, N> &B,
                   array<array<ELEM_TYPE, N>, N> &C) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      ELEM_TYPE sum = 0;
      for (size_t k = 0; k < N; ++k)
        sum += A[i][k] * B[j][k]; // row x row
      C[i][j] = sum;
    }
  }
}

// --- Configuration ---------------------------------------------------------

const size_t MATRIX_SIZE = 512;
const auto MAT_ITERATION = 3llu;

// Sum demo uses a larger array to exceed L3 cache and show clear difference.
// L1 is ~32 KB, L2 is ~256-512 KB, L3 is ~8-16 MB.
// We use 2048x2048 ints = 16 MB > typical L3.
const size_t SUM_SIZE = 2048;
const auto SUM_ITERATION = 5llu;

// --- Flat-array sum demos --------------------------------------------------

using ELEM_TYPE2 = int;

// Row-major: arr[i * N + j], j in inner loop -> stride 1 in memory
// When we read arr[i*N + 0], the CPU loads a 64-byte cache line holding
// arr[i*N + 0..15]. The next 15 accesses hit that same line -> fast.
long long sum_row_major() {
  static array<ELEM_TYPE2, SUM_SIZE * SUM_SIZE> arr{};
  static bool init = false;
  if (!init) {
    std::iota(arr.begin(), arr.end(), ELEM_TYPE2{0});
    init = true;
  }
  long long sum = 0;
  for (size_t i = 0; i < SUM_SIZE; ++i)
    for (size_t j = 0; j < SUM_SIZE; ++j)
      sum += arr[i * SUM_SIZE + j];
  return sum;
}

// Column-major: arr[i + N * j], j in inner loop -> stride N in memory
// When we read arr[0 + N*0], the CPU loads arr[0..15]. But the next access
// is arr[0 + N*1] which is 2048*4 = 8192 bytes away -> likely a cache miss.
long long sum_col_major() {
  static array<ELEM_TYPE2, SUM_SIZE * SUM_SIZE> arr{};
  static bool init = false;
  if (!init) {
    std::iota(arr.begin(), arr.end(), ELEM_TYPE2{0});
    init = true;
  }
  long long sum = 0;
  for (size_t i = 0; i < SUM_SIZE; ++i)
    for (size_t j = 0; j < SUM_SIZE; ++j)
      sum += arr[i + SUM_SIZE * j];
  return sum;
}

// --- Main ------------------------------------------------------------------

int main() {
  // Large arrays on the static storage to avoid stack overflow
  static auto A{gen_arr<MATRIX_SIZE>()};
  static auto B{gen_arr<MATRIX_SIZE>()};
  static auto C{gen_arr<MATRIX_SIZE>()};

  cout << "=== Matrix Multiply (" << MATRIX_SIZE << "x" << MATRIX_SIZE
       << " double) ===\n";
  cout << "Each element of C requires reading one row of A and one "
          "column/row of B.\n\n";

  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < MAT_ITERATION; ++i)
      mult_col_wise<MATRIX_SIZE>(A, B, C);
    auto stop = high_resolution_clock::now();
    auto avg =
        static_cast<double>(duration_cast<nanoseconds>(stop - start).count()) /
        MAT_ITERATION;
    cout << "A * B   (B accessed column-wise, stride " << MATRIX_SIZE
         << "): " << avg / 1e6 << " ms\n";
  }
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < MAT_ITERATION; ++i)
      mult_row_wise<MATRIX_SIZE>(A, B, C);
    auto stop = high_resolution_clock::now();
    auto avg =
        static_cast<double>(duration_cast<nanoseconds>(stop - start).count()) /
        MAT_ITERATION;
    cout << "A * B^T (B accessed row-wise,     stride 1): " << avg / 1e6
         << " ms  << faster\n";
  }

  cout << "\n=== Array Sum (" << SUM_SIZE << "x" << SUM_SIZE << " int, "
       << SUM_SIZE * SUM_SIZE * sizeof(ELEM_TYPE2) / (1024 * 1024)
       << " MB) ===\n";
  cout << "Both sum every element once, but in different order.\n\n";

  volatile long long sink = 0;
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < SUM_ITERATION; ++i)
      sink += sum_row_major();
    auto stop = high_resolution_clock::now();
    auto avg =
        static_cast<double>(duration_cast<nanoseconds>(stop - start).count()) /
        SUM_ITERATION;
    cout << "Row-major    (stride 1,  cache-friendly):   " << avg / 1e3
         << " us\n";
  }
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < SUM_ITERATION; ++i)
      sink += sum_col_major();
    auto stop = high_resolution_clock::now();
    auto avg =
        static_cast<double>(duration_cast<nanoseconds>(stop - start).count()) /
        SUM_ITERATION;
    cout << "Column-major (stride " << SUM_SIZE
         << ", cache-unfriendly): " << avg / 1e3 << " us  << slower\n";
  }
  (void)sink;
}
