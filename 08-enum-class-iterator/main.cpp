#include <iostream>
using namespace std;

enum class Colors { Red, Green, Blue, Colors_End };

Colors &operator++(Colors &c, int) {
  switch (c) {
  case Colors::Red:
    return c = Colors::Green;
  case Colors::Green:
    return c = Colors::Blue;
  case Colors::Blue:
    return c = Colors::Colors_End; // managing overflow
  }
}

int main() {
  for (auto v = Colors::Red; v != Colors::Colors_End; v++) {
    cout << static_cast<int>(v) << endl;
  }
}