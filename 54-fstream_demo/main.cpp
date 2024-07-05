#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void test_write() {
  cout << "test_write()\n";
  ofstream out("54-fstream_demo/test.txt");
  for (int i = 0; i < 10; ++i) {
    out << "Hello World!";
    out << i;
  }
  out.close();
}

void test_read_double() {
  cout << "test_read_double()\n";
  ifstream in("54-fstream_demo/input_double.txt");
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
}

void test_read() {
  cout << "test_read()\n";
  ifstream in("54-fstream_demo/test.txt");
  string sin;
  while (!in.eof()) {
    in >> sin;
    cout << sin << "\n";
  }
  in.close();
}

void read_txt_overcome_error(bool overcome) {
  cout << "read_txt_overcome_error(" << overcome << ")\n";
  ifstream in("54-fstream_demo/input_double_mix.txt");
  vector<double> arr;
  double tmp{0}, sum{0};
  string dummy;
  while (!in.eof()) {
    in >> tmp;
    if (overcome) {
      if (in.fail()) {
        in.clear();
        in >> dummy;
        continue;
      }
    }
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << "\n";
    sum += v;
  }
  cout << "sum: " << sum << "\n";
}

void fout_with_convention() {
  cout << "fout_with_convention()\n";
  ofstream out("54-fstream_demo/test.txt");
  for (int i = 0; i < 10; ++i) {
    out << "Hello World!\n";
    out << i << "\n";
  }
  out.close();
}

void fin_with_convension() {
  cout << "fin_with_convension()\n";
  ifstream in("54-fstream_demo/test.txt");
  string sin, dummy;
  int iin;
  for (int i = 0; i < 10; ++i) {
    getline(in, sin);
    in >> iin;
    getline(in, dummy);
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
    read_txt_overcome_error(true);
    // read_txt_overcome_error(false);
  }

  {
    fout_with_convention();
    fin_with_convension();
  }
}