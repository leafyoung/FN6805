// https://replit.com/@YeKunlun/36-linenumber?v=1

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

void line_number_dynamic_opt1(size_t current, vector<size_t> &line_numbers);
void line_number_dynamic_opt2(size_t current, vector<size_t> &line_numbers);

void line_number_static(int /*current*/, vector<size_t> &line_numbers) {
  for (size_t i = 0, j = 1; i < line_numbers.size(); ++i, ++j) {
    line_numbers[i] = static_cast<int>(j);
  }
  return;

  // Why below could be wrong here?
  {
    line_numbers[0] = 0;
    for (size_t i = 1, j = 0; i < line_numbers.size(); ++i, ++j) {
      line_numbers[i] = ++line_numbers[j];
    }
    return;
  }
}

void line_number_dynamic(size_t current, vector<size_t> &line_numbers) {
  const auto current_1 = current - 1;
  for (size_t i = 0; i < line_numbers.size(); ++i) {
    const auto si = static_cast<int>(i);
    if (si < current_1) {
      line_numbers[i] = current_1 - si;
    } else if (si == current_1) {
      line_numbers[i] = current;
    } else {
      line_numbers[i] = si - current_1;
    }
  }
}

void line_number_dynamic_opt(size_t current, vector<size_t> &line_numbers) {
  const auto current_1 = current - 1;
  const auto ln_size = line_numbers.size();
  for (size_t i = 0; i < static_cast<size_t>(current_1); ++i) {
    line_numbers[i] = current_1 - static_cast<int>(i);
  }
  line_numbers[static_cast<size_t>(current_1)] = current;
  for (size_t i = static_cast<size_t>(current); i < ln_size; ++i) {
    line_numbers[i] = static_cast<int>(i) - current_1;
  }
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

void test_line_number(string msg,
                      function<void(int, vector<size_t> &)> line_number_gen) {
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
    line_number_dynamic_opt1(current_test_cases[i], lines_numbers3);

    auto lines_numbers4 = vector<size_t>(17, 0);
    line_number_dynamic_opt2(current_test_cases[i], lines_numbers4);

    if (lines_numbers1 != lines_numbers2) {
      throw logic_error("line_number_dynamic_opt is not static");
    }
    if (lines_numbers1 != lines_numbers3) {
      throw logic_error("line_number_dynamic_opt is not static");
    }
    if (lines_numbers1 != lines_numbers4) {
      throw logic_error("line_number_dynamic_opt is not static");
    }
  }
  cout << "line_number_dynamic tests passed!\n";
}

int main() {
  test_line_number("line_number_static: "s, line_number_static);
  test_line_number("line_number_dynamic: "s, line_number_dynamic);
  test_line_number("line_number_dynamic_opt: "s, line_number_dynamic_opt);

  test_line_number("line_number_dynamic_opt1: "s, line_number_dynamic_opt1);
  test_line_number("line_number_dynamic_opt2: "s, line_number_dynamic_opt2);

  test_static();
  test_dynamic();
}