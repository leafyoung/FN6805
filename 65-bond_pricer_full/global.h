#pragma once

#include <string>
using namespace std;

const bool DEBUG = false;

bool isclose(double a, double b, int power = -7);

int get_payment_count(string ps);

struct FixedRateBond {
  double face_value;
  double coupon_rate;
  double maturity;
  string ps;
  double ytm;
};
