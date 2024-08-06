#include <iostream>
#include <vector>
using namespace std;

void concat_vector(size_t length) {
  vector<double> x(length, 1.0);
  vector<double> y(length, 2.0);
  cout << "x before resize: " << x.back() << endl;
  x.resize(length * 2);
  cout << "x after resize: " << x.back() << endl;
  // use next(), not x.back()
  copy(y.begin(), y.end(), next(x.begin(), length));
  cout << "x after copy: " << x.back() << endl;
}

void test_vector() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  concat_vector(100);
}