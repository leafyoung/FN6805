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

    auto str = "Hello World!"s;                                // 12 character
    size_t size = str.size();                                  // 12
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t)); // 12
    wf.write(reinterpret_cast<char *>(&str[0]), size); // write 12 characters

    size = 10;
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t)); // 10
    for (int i = 0; i < 10; ++i) {
      wf.write(reinterpret_cast<char *>(&i), sizeof i); //
    }
    // When we have a vector to write.
    // 1. Initialize the vector
    vector<double> vec(size);
    int i = 0;
    for_each(vec.begin(), vec.end(), [&i](auto &x) {
      x = (i + 1) * (i + 1);
      ++i;
    });

    // 2. Write the vector
    size = vec.size();
    wf.write(reinterpret_cast<char *>(&size), sizeof(size_t)); // 10
    for (auto v : vec) {
      wf.write(reinterpret_cast<char *>(&v), sizeof v); // value in its size
    }
    wf.close();
    cout << wf.good() << "\n";
  }

  {
    ifstream rf("50-binaryinoutfile/out_test.dat", ios::binary);
    size_t size;

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t)); // 12
    string str(size, 'a'); // 12 character long string
    rf.read(reinterpret_cast<char *>(&str[0]), size); // read into it
    cout << str << "\n";

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t)); // 10
    vector<size_t> vec(size); // 10 element long vector
    for (auto &v : vec) {     // auto with reference type
      rf.read(reinterpret_cast<char *>(&v),
              sizeof(int)); // read each element into it
    }
    copy(vec.begin(), vec.end(), ostream_iterator<int>(cout, ", "));
    cout << "\n";

    rf.read(reinterpret_cast<char *>(&size), sizeof(size_t)); // 10
    vector<double> vec2(size); // 10 element long vector
    for (auto &v : vec2) {
      rf.read(reinterpret_cast<char *>(&v),
              sizeof v); // read each element into it
    }
    copy(vec2.begin(), vec2.end(), ostream_iterator<double>(cout, ", "));
    cout << "\n";

    rf.close();
  }
}

int main() {
  cout << boolalpha;

  test_binary();
}