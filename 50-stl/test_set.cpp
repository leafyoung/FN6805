#include "tests.h"
#include <cassert>
#include <iostream>
#include <set>

using namespace std;

void test_set1() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  cout << "test_set1"
       << "\n";
  set<string> visitors;

  cout << boolalpha;

  visitors.insert("Alice");
  visitors.insert("Bob");
  visitors.insert("Charlie");
  visitors.erase("Alice");
  std::cout << visitors.count("Alice") << "\n";
  std::cout << visitors.count("Charlie") << "\n";

  // if set.find(value) == set.end(), meaning not found
  cout << (visitors.find("Bob") == visitors.end()) << "\n";

  // if .count == 1, meaning found
  std::cout << visitors.count("Bob") << "\n";
  assert(visitors.count("Bob") == 1);
}

void test_set2() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  cout << "test_set2"
       << "\n";
  set<int> numbers;
  for (int i = 1; i <= 10; ++i) {
    numbers.insert(i);
  }
  numbers.erase(5);

  cout << "numbers: ";
  for (auto it : numbers) {
    cout << it << ", ";
  }
  cout << "\n";

  auto it = numbers.find(5);
  if (it != numbers.end()) {
    cout << "Found"
         << "\n";
  } else {
    cout << "Not found"
         << "\n";
  }
}
