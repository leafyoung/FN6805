#include <cmath>
#include <limits>
#include <string>
using namespace std;

bool isclose(double a, double b, int power) {
  // return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) *
  // numeric_limits<double>::epsilon());
  return fabs(a / b - 1) < pow(10, power);
}

int get_payment_count(string ps) {
  if (ps == "A") {
    return 1;
  } else if (ps == "S") {
    return 2;
  } else if (ps == "Q") {
    return 4;
  } else if (ps == "M") {
    return 12;
  }
  return nan("1");
}
