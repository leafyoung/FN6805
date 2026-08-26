#include <iostream>
using namespace std;

int main(int argc, const char *argv[]) {
  int anInt = 0;
  float aFloat = 0.0;
  double aDouble = 0.0;

  cout << '\n'
       << "Memory lego bricks, int:    " << sizeof(anInt) << '\n'
       << "Memory lego bricks, float:  " << sizeof(aFloat) << '\n'
       << "Memory lego bricks, double: " << sizeof(aDouble) << '\n';

  cout.setf(ios::hex);
  cout.unsetf(ios::dec);

  aFloat = 3.3;
  aDouble = 3.4;

  cout << '\n'
       << "&anInt:   " << &anInt << '\n'
       << "&aFloat:  " << &aFloat << '\n'
       << "&aDouble: " << &aDouble << '\n'
       << (reinterpret_cast<size_t>(&anInt) - reinterpret_cast<size_t>(&aFloat))
       << '\n'
       << (reinterpret_cast<size_t>(&aFloat) -
           reinterpret_cast<size_t>(&aDouble))
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