#include <functional>
#include <iostream>
#include <random>
#include <vector>
using namespace std;

bool Condition(vector<int> v) {
  for (int i = 0; i < v.size(); i++)
    if (v[i] % 3 == 0)
      return true;
  return false;
}

class Test {
  vector<int> w;

public:
  Test(vector<int> w1) { w = w1; };

  bool test(function<bool(vector<int>)> condition) { return condition(w); }
};

int main() {
  // create random vector

  vector<int> v;
  for (int i = 0; i < 500; i++)
    v.push_back(rand());

  // check directly if condition is satisfied:
  cout << "Is there an entry divisible by 1000?: " << Condition(v) << '\n';

  // check through member function of Test if condition is satisfied:
  Test T(v);
  cout << "Is there an entry divisible by 1000?: " << T.test(Condition) << '\n';
}
