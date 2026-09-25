#include <cmath>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

int main() {
  cout << boolalpha;

  vector<int> v{1, 2, 3};
  auto it = v.begin();

  cout << *(it + 2) << '\n';

  list<int> l{1, 2, 3};
  auto it2 = l.begin();
  // cout << *(it + 2) << '\n'; // compiler error
  // 2nd element in the list
  cout << *(++it2) << '\n';

  advance(it2, 2); // equivalent to it2 + 2
  cout << (it2 == l.end()) << '\n';

  // DELIBERATE UB DEMO: it2 == l.end(); dereferencing end() is undefined
  // behaviour (debug builds abort here). Never do this in real code.
  cout << *it2 << '\n';

  // the 3rd element
  cout << *(prev(it2, 1)) << '\n';
  // the 2nd element
  cout << *(prev(it2, 2)) << '\n';
  // the 3rd element
  cout << *(next(prev(it2, 2), 1)) << '\n';

  // DELIBERATE UB DEMO: the non-existent 5th element, past end().
  cout << *(next(it2, 1)) << '\n';

  cout << "insertion" << '\n';

  {
    list<int> vec{1, 2, 3};
    // insert from a copy: a source range from vec itself is UB
    auto copy = vec;
    // only for vector/array
    // vec.insert(vec.begin() + 2, copy.begin(), copy.end());

    // working for both list/vector/array
    vec.insert(next(vec.begin(), 2), copy.begin(), prev(copy.end(), 1));

    for (auto v : vec) {
      cout << v << ", ";
    }
    cout << '\n';
  }

  {
    vector<int> vec{1, 2, 3};

    for (int i = 0; i < 3; ++i) {
      cout << vec[i] << '\n';
    }
  }
  {
    list<int> lst{1, 2, 3};

    int i = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
      cout << i << ", " << *it << '\n';
      i++;
    }

    for (auto it = lst.begin(); it != lst.end(); ++it) {
      cout << distance(lst.begin(), it) << ", " << *it << '\n';
    }
  }

  {
    cout << "vector" << '\n';
    vector<int> vec{1, 2, 3};
    // advance(it, 2) past end() is UB for vector: step by index instead
    for (size_t i = 0; i < vec.size(); i += 2) {
      cout << vec[i] << '\n';
    }

    cout << "list" << '\n';
    list<int> lst{1, 2, 3};

    // next for list is O(n) operation, a performance trap.
    for (auto it = lst.begin(); it != lst.end(); advance(it, 2)) {
      cout << *it << '\n';
      if (next(it, 1) == lst.end() || next(it, 2) == lst.end()) {
        break;
      }
    }

    cout << "list2:" << '\n';
    // use of external tracking variable i is better
    int i = 0;
    auto const n_elem = static_cast<int>(ceil(lst.size() / 2.0));
    for (auto it = lst.begin(); i < n_elem; ++i) {
      cout << *it << '\n';
      if (i + 1 < n_elem)
        advance(it, 2); // never step past end()
    }
  }
}