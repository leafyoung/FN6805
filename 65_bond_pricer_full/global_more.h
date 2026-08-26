#pragma once
#include <string>
using namespace std;

struct FloatingRateBond {
  double face_value;
  double maturity;
  string ps;
  double ytm;
  double spread;
  bool is_loan; // true: no final principal payment, false final principal
                // payment
};

struct AmortizationBond {
  double principal;
  double maturity;
  string ps;
  double yield;
};
