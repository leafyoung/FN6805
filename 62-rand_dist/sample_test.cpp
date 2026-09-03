#include <stdexcept>

using namespace std;

// input data
struct Param {
  int a;
  double b;
};

// output: we can return one or multiple
struct Result {
  int r;
};

// process
Result produce(Param p) { return {0}; }

// test
void test_produce() {
  produce({.a = 0, .b = 3.0}).r == 0 ? true : throw logic_error("wrong result");
  // Or any problem-specific test
}
