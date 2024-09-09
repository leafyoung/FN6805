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

void copy_to_itself() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  {
    cout << "Good (Resize and then copy) \n";
    vector<int> v{1, 2, 3, 4, 5};
    // resize the container first
    v.resize(v.size() + 3);
    copy(v.begin(), next(v.begin(), 3), next(v.begin(), 5));
    for (auto x : v) {
      cout << x << ",";
    }
    cout << "\n";
  }
  {
    cout << "Wrong (copy while resizing) \n";
    vector<int> v{1, 2, 3, 4, 5};
    copy(v.begin(), next(v.begin(), 3), back_inserter(v));
    for (auto x : v) {
      cout << x << ",";
    }
    cout << "\n";
  }
}

void test_vector() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";
  concat_vector(100);

  copy_to_itself();
}