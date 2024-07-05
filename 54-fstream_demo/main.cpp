#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

void read_txt_with_error() {
  cout << "read_txt_with_error()\n";

  ifstream in("54-fstream_demo/input_double_mix.txt");
  vector<double> arr;
  double tmp, sum{0};
  string dummy;
  while (!in.eof()) {
    in >> tmp;
    if (in.fail()) {
      in.clear();
      in >> dummy;
      continue;
    }
    arr.push_back(tmp);
  }
  in.close();
  for (auto v : arr) {
    cout << v << endl;
    sum += v;
  }
  cout << "sum: " << sum << endl;
}

void fout() {
  cout << "fout()\n";

  ofstream out("54-fstream_demo/test.txt");
  for (int i = 0; i < 10; ++i) {
    out << "Hello World!";
    out << i;
  }
  out.close();
}

void fin() {
  cout << "fin()\n";

  ifstream in("54-fstream_demo/test.txt");

  string sin;
  while (!in.eof()) {
    in >> sin;
    cout << sin << endl;
  }

  in.close();
}

void fout_with_convention() {
  cout << "fout_with_convention()\n";

  ofstream out("54-fstream_demo/test.txt");
  for (int i = 0; i < 10; ++i) {
    out << "Hello World!" << endl;
    out << i << endl;
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
    cout << sin << endl << iin << endl;
  }
  in.close();
}

int main() {
  { read_txt_with_error(); }

  {
    fout();
    fin();
  }
  {
    fout_with_convention();
    fin_with_convension();
  }
}