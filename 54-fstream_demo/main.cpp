#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void test_write() {
  cout << "test_write()\n";
  ofstream out("54-fstream_demo/test.txt");
  for (int i = 1; i < 11; ++i) {
    out << "Hello World!" << i << "\n";
  }
  out.close();
}

void test_read() {
  cout << "test_read()\n";
  ifstream in("54-fstream_demo/test.txt");
  string sin;
  while (!in.eof()) {
  // while (in >> sin) {
    in >> sin;
    cout << sin << "\n";
  }
  in.close();
}

void test_read_double() {
  cout << "test_read_double()\n";
  ifstream in("54-fstream_demo/input_double.txt");
  vector<double> arr;
  double tmp{0};
  while (!in.eof()) {
    in >> tmp;
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << "\n";
  }
}

void read_txt_overcome_error() {
  cout << "read_txt_overcome_error()\n";
  ifstream in("54-fstream_demo/input_double_mix.txt");
  vector<double> arr;
  double tmp{0};
  string dummy;
  while (!in.eof()) {
    in >> tmp;
    if (in.fail()) { // if fail
      in.clear(); // clear
      in >> dummy; // read as string
      continue; // read the next
    }
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << "\n";
  }
}

void fout_with_convention() {
  cout << "fout_with_convention()\n";
  ofstream out("54-fstream_demo/test2.txt");
  for (int i = 1; i < 11; ++i) {
    out << "Hello World!\n";
    out << i << "\n";
  }
  out.close();
}

void fin_with_convension() {
  cout << "fin_with_convension()\n";
  ifstream in("54-fstream_demo/test2.txt");
  string sin, dummy;
  int iin;
  for (int i = 0; i < 10; ++i) {
    getline(in, sin); // read one string one line
    in >> iin; // read one int one line
    getline(in, dummy); // consumes the rest of the line
    cout << sin << "\n" << iin << "\n";
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
    fout_with_convention();
    fin_with_convension();
  }
}