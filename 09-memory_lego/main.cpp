#include <iostream>
using namespace std;

int main() {
  int an_int = 0;
  float a_float = 0.0;
  double a_double = 0.0;

  cout << '\n'
       << "Memory lego bricks, int:    " << sizeof(an_int) << '\n'
       << "Memory lego bricks, float:  " << sizeof(a_float) << '\n'
       << "Memory lego bricks, double: " << sizeof(a_double) << '\n';

  cout.setf(ios::hex);
  cout.unsetf(ios::dec);

  a_float = 3.3;
  a_double = 3.4;

  cout << '\n'
       << "&an_int:   " << &an_int << '\n'
       << "&a_float:  " << &a_float << '\n'
       << "&a_double: " << &a_double << '\n'
       << (reinterpret_cast<size_t>(&an_int) - reinterpret_cast<size_t>(&a_float))
       << '\n'
       << (reinterpret_cast<size_t>(&a_float) -
           reinterpret_cast<size_t>(&a_double))
       << '\n'
       << '\n';

  //
  // |DDDDDDDD|FFFF|IIII|
  // |0.0     |0.0 |0   |
  // When we expand 1 byte to 8 bits to 2 hex.
  // dddddddddddddddd|eeeeeeee|eeee|eeee|ffffffffffffffff
  // 0123456789abcdef|01234567|89ab|cdef|0123456789abcdef

  return 0;
}