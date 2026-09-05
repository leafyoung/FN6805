#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void test_write() {
  cout << "test_write()\n";
  ofstream out("./test.txt");
  for (int i = 1; i < 11; ++i) {
    out << "Hello World!" << i << '\n';
  }
  out.close();
}

void test_read() {
  cout << "test_read()\n";
  ifstream in("./test.txt");
  string sin;
  while (in >> sin) {
    cout << sin << '\n';
  }
  in.close();
}

void test_read_double() {
  cout << "test_read_double()\n";
  ifstream in("./input_double.txt");
  vector<double> arr;
  double tmp{0};
  while (in >> tmp) {
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << '\n';
  }
}

void read_txt_overcome_error() {
  cout << "read_txt_overcome_error()\n";
  ifstream in("./input_double_mix.txt");
  vector<double> arr;
  double tmp{0};
  string dummy;
  while (true) {
    if (in >> tmp) {
      arr.push_back(tmp);
    } else if (in.eof()) {
      break;
    } else {
      in.clear();
      in >> dummy;
    }
  }
  in.close();
  for (auto v : arr) {
    cout << v << '\n';
  }
}

void write_with_convention() {
  cout << "write_with_convention()\n";
  ofstream out("./test2.txt");
  for (int i = 1; i < 11; ++i) {
    out << "Hello World!\n";
    out << i << '\n';
  }
  out.close();
}

void read_with_convension() {
  cout << "read_with_convension()\n";
  ifstream in("./test2.txt");
  string sin, dummy;
  int iin;
  for (int i = 0; i < 10; ++i) {
    getline(in, sin);   // read one string one line
    in >> iin;          // read one int one line
    getline(in, dummy); // consumes the rest of the line
    // dummy is actually empty, but we need it to finish the reading of the line
    cout << "dummy: " << (dummy == "" ? "Yes" : "No") << '\n';
    cout << sin << '\n' << iin << '\n';
  }
  in.close();
}

int main() {
  {
    test_write();
    test_read();
    test_read_double();
  }

  {
    read_txt_overcome_error();
  }

  {
    write_with_convention();
    read_with_convension();
  }
}