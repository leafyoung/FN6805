// https://replit.com/@YeKunlun/50-binaryinoutfile?v=1

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
using namespace std;

void test_read_txt() {
  cout << "test_read_txt(): input.txt"
       << "\n";
  ifstream in("50-binaryinoutfile/input.txt");
  vector<double> arr;
  double tmp;
  while (!in.eof()) {
    in >> tmp;
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << "\n";
  }
} // namespace std::filesystemlpathvoidtest_read_txt()

void test_read_mix() {
  cout << "test_read_mix(): input.txt"
       << "\n";
  ifstream in("50-binaryinoutfile/input_mix.txt");
  vector<double> arr;
  double tmp;
  double sum{0};
  while (!in.eof()) {
    in >> tmp;
    if (in.fail()) {
      break;
    }
    arr.push_back(tmp);
    sum += tmp;
  }
  in.close();
  for (auto v : arr) {
    cout << v << "\n";
  }
  cout << "sum: " << sum << "\n";
}

void test_mix_txt() {
  cout << "test_mix_txt()"
       << "\n";
  ofstream out("50-binaryinoutfile/out_test.txt");
  out << "Hello World!"
      << "\n";
  for (int i = 0; i < 10; i++) {
    out << i << " ";
  }
  out.close();

  ifstream in_file("50-binaryinoutfile/out_test.txt");
  string file_string;
  getline(in_file, file_string);
  vector<int> file_int(10);
  for (int i = 0; i < 10; i++) {
    in_file >> file_int[i];
  }
  in_file.close();

  cout << file_string << "\n";
  for (auto v : file_int) {
    cout << v << ", ";
  }
  cout << "\n";
}

struct file_data {
  string str;
  vector<int> vec;
};

void test_binary() {
  cout << "test_binary()"
       << "\n";
  cout << boolalpha;
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

    // if we have a vector to write, we can do the similar operation
    // first, initialize the vector
    vector<int> vec(size);
    for (int i = 0; i < size; i++) {
      vec[i] = (i + 1) * (i + 1);
    }

    // second, write the vector
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
  test_read_txt();

  test_read_mix();

  test_mix_txt();

  test_binary();
}