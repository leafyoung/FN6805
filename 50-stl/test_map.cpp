#include "tests.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

struct Student {
  string name;
  int score;
};

void test_map() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  std::map<char, int> my_map;
  my_map['a'] = 0;
  my_map['b'] = 3;
  my_map['c'] = 40;

  std::map<char, int> second_map{{'a', 3}, {'b', 4}};
  cout << second_map.size() << "\n";

  // Since C++17
  for (const auto [key, val] : my_map) {
    std::cout << key << ": " << val << '\n';
  }
  // Use reference type to update the value
  for (auto &[key, value] : my_map) {
    // key++; // not allowed to modify key
    ++value;
  }
  // use iterator
  for (auto it = my_map.begin(); it != my_map.end(); ++it) {
    cout << it->first << ", " << it->second << "\n";
  }

  // find the key
  auto it = my_map.find('c');
  // if .find(key) == .end(), meaning not found.
  if (it != my_map.end())
    // print the value of the key
    cout << it->second << "\n";

  // With customized comparison for the order of key
  // Method 1. use lambda
  {
    auto comp = [](const string &a, const string &b) {
      return a.length() < b.length();
    };
    map<string, string, decltype(comp)> my_map(comp);

    my_map["1"] = "a";
    my_map["three"] = "b";
    my_map["two"] = "c";
    my_map["fouuur"] = "d";

    for (auto const &kv : my_map)
      cout << kv.first << "\n";
  }

  // Method 2: use struct with operator()
  {
    struct cmp {
      bool operator()(const Student &a, const Student &b) const {
        return a.score > b.score; // Descending order }
      }
    };

    map<Student, string, cmp> students{{{"Bob", 90}, "A"},
                                       {{"Alice", 96}, "A+"}};
    for (auto &[key, value] : students) {
      cout << key.name << ": " << value << "\n";
    }
  }
}