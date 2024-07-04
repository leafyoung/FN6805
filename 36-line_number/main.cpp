// https://replit.com/@YeKunlun/36-linenumber?v=1

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;

void line_number_static(int current, vector<int> &line_numbers) {
  if (current > line_numbers.size()) {
    line_numbers.resize(current);
  }
  for (int i = 0, line = 1; i < line_numbers.size(); ++i, ++line) {
    line_numbers[i] = line;
  }
  return;

  // Why below could be wrong here?
  {
    line_numbers[0] = 0;
    for (int i = 1, j = 0; i < line_numbers.size(); ++i, ++j) {
      line_numbers[i] = ++line_numbers[j];
    }
    return;
  }
}

void line_number_dynamic(int current, vector<int> &line_numbers) {
  const auto current_1 = current - 1;
  if (current > line_numbers.size()) {
    line_numbers.resize(current);
  }
  for (int i = 0; i < line_numbers.size(); ++i) {
    if (i < current_1) {
      line_numbers[i] = current_1 - i;
    } else if (i == current_1) {
      line_numbers[i] = current;
    } else {
      line_numbers[i] = i - current_1;
    }
  }
}

void line_number_dynamic_opt(int current, vector<int> &line_numbers) {
  const auto current_1 = current - 1;
  const auto ln_size = line_numbers.size();
  if (current > ln_size) {
    line_numbers.resize(current);
  }
  for (int i = 0; i < current_1; ++i) {
    line_numbers[i] = current_1 - i;
  }
  line_numbers[current_1] = current;
  for (int i = current; i < ln_size; ++i) {
    line_numbers[i] = i - current_1;
  }
}

void print_vector(vector<int> vs) {
  for (int i = 0; i < vs.size(); ++i) {
    cout << vs[i] << ", ";
  }
  cout << "\n";
}

void test_line_number(string msg,
                      function<void(int, vector<int> &)> line_number_gen) {
  cout << msg << "\n";
  auto x3 = vector<int>(30, 0);
  auto test_cases = vector<int>{1, 10, 20, 30, 34};
  for (int i = 0; i < test_cases.size(); ++i) {
    line_number_gen(test_cases[i], x3);
    print_vector(x3);
  }
}

int main() {
  test_line_number("line_number_static: "s, line_number_static);

  test_line_number("line_number_dynamic: "s, line_number_dynamic);

  test_line_number("line_number_dynamic_opt: "s, line_number_dynamic_opt);
}