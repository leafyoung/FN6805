#include <iostream>
#include <vector>
using std::vector;

// Show me can
int three_n_plus_1_problem(unsigned int n) {
  if (n == 1) {
    return true;
  }
  if (n % 2 == 0) {
    n /= 2;
  } else {
    n = 3 * n + 1;
  }
  return three_n_plus_1_problem(n);
}

// Show me how many times
int three_n_plus_1_problem(unsigned int n, unsigned int num) {
  if (n == 1) {
    return num + 1;
  }
  if (n % 2 == 0) {
    n /= 2;
  } else {
    n = 3 * n + 1;
  }
  return three_n_plus_1_problem(n, num + 1);
}

// Show me what's the sequence
int three_n_plus_1_problem(unsigned int n, vector<int> &seq) {
  seq.push_back(n);
  if (n == 1) {
    return true;
  }
  if (n % 2 == 0) {
    n /= 2;
  } else {
    n = 3 * n + 1;
  }
  return three_n_plus_1_problem(n, seq);
}

// The sequence result is complex, so we need to write a function to do so
void test_seq(int n) {
  vector<int> seq;
  std::cout << three_n_plus_1_problem(n, seq) << "\n";
  std::cout << seq.size() << ": ";
  for (auto v : seq) {
    std::cout << v << ", ";
  }
  std::cout << "\n";
  seq.clear();
}

int main() {
  std::cout << three_n_plus_1_problem(10) << "\n";
  std::cout << three_n_plus_1_problem(100) << "\n";
  std::cout << three_n_plus_1_problem(1000) << "\n";

  std::cout << three_n_plus_1_problem(10, 0) << "\n";
  std::cout << three_n_plus_1_problem(100, 0) << "\n";
  std::cout << three_n_plus_1_problem(1000, 0) << "\n";

  test_seq(10);
  test_seq(100);
  test_seq(1000);
}