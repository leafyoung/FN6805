#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

void concat_vector(size_t length) {
  vector<double> x(length, 1.0);
  vector<double> y(length, 2.0);
  cout << "x before resize: " << x.back() << '\n';
  x.resize(length * 2);
  cout << "x after resize: " << x.back() << '\n';
  // use next(), not x.back()
  copy(y.begin(), y.end(), next(x.begin(), length));
  cout << "x after copy: " << x.back() << '\n';
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
    cout << '\n';
  }
  {
    cout << "Wrong (copy while resizing) \n";
    vector<int> v{1, 2, 3, 4, 5};
    copy(v.begin(), next(v.begin(), 3), back_inserter(v));
    for (auto x : v) {
      cout << x << ",";
    }
    cout << '\n';
  }
}

// back_inserter: output iterator adaptor that calls push_back on every write.
// Useful when the output size is not known in advance.
void test_back_inserter() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  vector<int> src{-3, 1, -1, 4, 2};
  vector<int> pos;
  copy_if(src.begin(), src.end(),
          back_inserter(pos), // grows pos as needed
          [](int x) { return x > 0; });
  for (auto x : pos)
    cout << x << " "; // 1 4 2
  cout << '\n';
}

// Iterator invalidation demo.
// Rule: do not modify a container while holding its iterators
// unless you use the return value of erase/insert.
void test_erase_while_iterating() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";

  // WRONG: erasing with a stale iterator (undefined behaviour)
  // vector<int> v{1, 2, 3, 4, 5};
  // for (auto it = v.begin(); it != v.end(); ++it)
  //   if (*it % 2 == 0) v.erase(it);  // 'it' is now invalid!

  // CORRECT: erase returns the next valid iterator
  vector<int> v{1, 2, 3, 4, 5};
  for (auto it = v.begin(); it != v.end();) {
    if (*it % 2 == 0)
      it = v.erase(it); // erase returns next valid iterator
    else
      ++it;
  }
  for (auto x : v)
    cout << x << " "; // 1 3 5
  cout << '\n';
}

void test_vector() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";
  concat_vector(100);

  copy_to_itself();
  test_back_inserter();
  test_erase_while_iterating();
}