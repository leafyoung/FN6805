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

  cout << *it2 << '\n'; // invalid access the 4th element. we shall avoid

  // the 3rd element
  cout << *(prev(it2, 1)) << '\n';
  // the 2nd element
  cout << *(prev(it2, 2)) << '\n';
  // the 3rd element
  cout << *(next(prev(it2, 2), 1)) << '\n';

  // the non-existed 5th element, invalid access.
  cout << *(next(it2, 1)) << '\n';

  cout << "insertion" << '\n';

  {
    list<int> vec{1, 2, 3};
    // only for vector/array
    // vec.insert(vec.begin() + 2, vec.begin(), vec.end());

    // working for both list/vector/array
    vec.insert(next(vec.begin(), 2), vec.begin(), prev(vec.end(), 1));

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
    for (auto it = vec.begin(); it < vec.end(); advance(it, 2)) {
      cout << *it << '\n';
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
    for (auto it = lst.begin(); i < n_elem; advance(it, 2), ++i) {
      cout << *it << '\n';
    }
  }
}