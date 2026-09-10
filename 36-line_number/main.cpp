// https://github.com/leafyoung/FN6805/tree/main/36-line_number

#include "line_number.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

void line_number_static(size_t /*current*/, vector<size_t> &line_numbers) {
  for (size_t i = 0, j = 1; i < line_numbers.size(); ++i, ++j) {
    line_numbers[i] = j;
  }
  return;

  // Below code are currently not used, but I want to keep them here to ask why
  // below could be wrong to use?
  {
    line_numbers[0] = 0;
    for (size_t i = 1, j = 0; i < line_numbers.size(); ++i, ++j) {
      line_numbers[i] = ++line_numbers[j];
    }
    return;
  }
}

void line_number_dynamic(size_t current, vector<size_t> &line_numbers) {
  // BASELINE: Single loop with branching
  // This is the straightforward implementation - easy to understand but
  // has branching inside the loop which impacts performance.
  const auto current_1 = current - 1;
  for (size_t i = 0; i < line_numbers.size(); ++i) {
    if (i < current_1) {
      line_numbers[i] = current_1 - i;
    } else if (i == current_1) {
      line_numbers[i] = current;
    } else {
      line_numbers[i] = i - current_1;
    }
  }
}

void line_number_dynamic_opt(size_t current, vector<size_t> &line_numbers) {
  // OPTIMIZED: Three separate loops with ZERO branches inside
  //
  // Performance advantages over line_number_dynamic:
  //
  // 1. NO BRANCH MISREDICTIONS:
  //    - Each loop has no conditional branches inside
  //    - CPU can predict perfectly → no pipeline flushes
  //
  // 2. VECTORIZATION:
  //    - Compiler can SIMD-vectorize these simple loops
  //    - Processes multiple elements per CPU cycle
  //
  // 3. CLEAR STRUCTURE:
  //    - Three regions are explicit: left of current, current, right of current
  //    - Easy to understand and verify correctness
  //
  // For n=17 elements: ~17 operations, 0 mispredictions → FAST
  //
  const auto current_1 = current - 1;
  const auto ln_size = line_numbers.size();
  for (size_t i = 0; i < current_1; ++i) {
    line_numbers[i] = current_1 - i;
  }
  line_numbers[current_1] = current;
  for (size_t i = current; i < ln_size; ++i) {
    line_numbers[i] = i - current_1;
  }
}

void line_number_dynamic_stl(size_t current, vector<size_t> &line_numbers) {
  // STL ALGORITHM VERSION using std::generate with lambdas
  //
  // Same performance characteristics as line_number_dynamic_opt:
  // - Zero branches in the generated code (compiler unfolds the lambdas)
  // - SIMD-vectorizable
  // - More declarative/functional style
  //
  // Trade-offs:
  // + Idiomatic modern C++
  // + Clear intent (generate sequences)
  // - Slightly more verbose due to lambda captures
  // - May be less familiar to beginners than raw loops
  if (current > line_numbers.size()) {
    line_numbers.resize(current);
  }
  const auto current_1 = current - 1;
  size_t i = 0;
  generate(line_numbers.begin(), line_numbers.begin() + current - 1,
           [current_1, &i]() { return current_1 - i++; });
  line_numbers[current_1] = current;
  i = 1;
  generate(line_numbers.begin() + current, line_numbers.end(),
           [&i]() { return i++; });
}

void print_vector(const vector<size_t> &vs, size_t high_location) {
  for (size_t i = 0; i < vs.size(); ++i) {
    if (i == high_location - 1) {
      cout << "[" << vs[i] << "], ";
    } else {
      cout << vs[i] << ", ";
    }
  }
  cout << '\n';
}

void test_line_number(
    string msg, function<void(size_t, vector<size_t> &)> line_number_gen) {
  cout << msg << '\n';
  auto lines_numbers = vector<size_t>(17, 0);
  auto current_test_cases = vector<size_t>{1, 2, 7, 12, 17};
  for (size_t i = 0; i < current_test_cases.size(); ++i) {
    cout << "Test: " << current_test_cases[i] << '\n';
    line_number_gen(current_test_cases[i], lines_numbers);
    print_vector(lines_numbers, current_test_cases[i]);
  }
  cout << '\n';
}

void test_static() {
  auto lines_numbers1 = vector<size_t>(17, 0);
  auto current_test_cases = vector<size_t>{1, 2, 7, 12, 17};
  line_number_static(current_test_cases[0], lines_numbers1);
  for (size_t i = 1; i < current_test_cases.size(); ++i) {
    auto lines_numbers2 = vector<size_t>(17, 0);
    line_number_static(current_test_cases[i], lines_numbers2);
    if (lines_numbers1 != lines_numbers2) {
      throw logic_error("line_number_static is not static");
    }
  }
  cout << "line_number_static test passed!\n";
}

void test_dynamic() {
  auto current_test_cases = vector<size_t>{1, 2, 7, 12, 17};
  for (size_t i = 0; i < current_test_cases.size(); ++i) {
    auto lines_numbers1 = vector<size_t>(17, 0);
    line_number_dynamic(current_test_cases[i], lines_numbers1);

    auto lines_numbers2 = vector<size_t>(17, 0);
    line_number_dynamic_opt(current_test_cases[i], lines_numbers2);

    auto lines_numbers3 = vector<size_t>(17, 0);
    line_number_dynamic_stl(current_test_cases[i], lines_numbers3);

    if (lines_numbers1 != lines_numbers2) {
      throw logic_error("line_number_dynamic_opt produces different output");
    }
    if (lines_numbers1 != lines_numbers3) {
      throw logic_error("line_number_dynamic_stl produces different output");
    }
  }
  cout << "line_number_dynamic tests passed!\n";
}

int main() {
  test_line_number("line_number_static: "s, line_number_static);
  test_line_number("line_number_dynamic (BASELINE - with branching): "s, line_number_dynamic);
  test_line_number("line_number_dynamic_opt (OPTIMIZED - zero branches): "s, line_number_dynamic_opt);
  test_line_number("line_number_dynamic_stl (STL with std::generate): "s, line_number_dynamic_stl);

  test_static();
  test_dynamic();
}