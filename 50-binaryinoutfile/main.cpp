// https://replit.com/@YeKunlun/50-binaryinoutfile?v=1

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
using namespace std;

struct file_data {
  string str;
  vector<int> vec;
};

void test_binary() {
  cout << "test_binary()\n";
  {
    ofstream wf("50-binaryinoutfile/out_test.dat", ios::out | ios::binary);

    auto str = "Hello World!"s;
    size_t size = str.size();
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t));
    wf.write(reinterpret_cast<char *>(&str[0]), size);

    size = 10;
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t));
    for (int i = 0; i < 10; i++) {
      wf.write(reinterpret_cast<char *>(&i), sizeof i);
    }

    // When we have a vector to write.
    // 1. Initialize the vector
    vector<int> vec(size);
    for (int i = 0; i < size; i++) {
      vec[i] = (i + 1) * (i + 1);
    }

    // 2. Write the vector
    size = vec.size();
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t));
    for (auto v : vec) {
      wf.write(reinterpret_cast<char *>(&v), sizeof v);
    }
    wf.close();
    cout << wf.good() << "\n";
  }

  {
    ifstream rf("50-binaryinoutfile/out_test.dat", ios::binary);
    size_t size;

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    string str(size, 'a');
    rf.read(reinterpret_cast<char *>(&str[0]), size);

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    vector<int> vec(size);
    for (auto &v : vec) {
      rf.read(reinterpret_cast<char *>(&v), sizeof(int));
    }
    for (auto v : vec) {
      cout << v << ", ";
    }
    cout << "\n";

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    vector<int> vec2(size);
    for (auto &v : vec2) {
      rf.read(reinterpret_cast<char *>(&v), sizeof(int));
    }
    for (auto v : vec2) {
      cout << v << ", ";
    }
    cout << "\n";

    // copy(vec.begin(), vec.end(), ostream_iterator<int>(cout, ", "));
    rf.close();
  }
}

int main() {
  cout << boolalpha;

  test_binary();
}