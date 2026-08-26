#include "bond_zero.h"
#include "global.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

double bond_price_zero(FixedRateBond frb, const int today, const RateCurve IC,
                       double ic_spread) {
  if (today <= 0)
    throw logic_error("today > 0");

  int payment_count = get_payment_count(frb.ps);
  int n = frb.maturity * payment_count;
  vector<double> cf(n, frb.coupon_rate / payment_count * frb.face_value),
      df(n, 0);
  cf[n - 1] += frb.face_value;
  double pv = 0;
  double dcf;
  int coupon_day = 0;
  for (int i = 0; i < n; i++) {
    coupon_day = today + (i + 1.0) / payment_count * 365.0;
    df[i] = get_df(today, coupon_day, IC, today, ic_spread);
    dcf = df[i] * cf[i];
    pv += dcf;
    DEBUG &&cout << i << ": " << coupon_day << ", " << df[i] << ", " << cf[i]
                 << ", " << dcf << ", " << pv << "," << ic_spread << endl;
  }
  return pv;
}

void test_bond_price_zero() {
  cout << "test_bond_price_zero: " << endl;

  {} {
    // flat rate - result are close to 100.
    auto v00f = bond_price_zero({100, 0.03, 2, "A", 0.03}, 41082,
                                {{41082, 0}, {41083, 3.0}}, 0.0);
    cout << "v00f1:" << v00f << endl;

    auto v00f2 = bond_price_zero({100, 0.04, 10, "M", 0.04}, 41082,
                                 {{41082, 0}, {41083, 4.0}}, 0.0);
    cout << "v00f2:" << v00f2 << endl;
  }

  {
    auto v00 = bond_price_zero({100, 0.02, 4, "Q", 0.02}, 41082, IC, 0.0);
    cout << "v00:" << v00 << endl; // v00:100.579

    auto v01 = bond_price_zero({100, 0.03, 2, "S", 0.03}, 41082, IC, 0.0);
    cout << "v01:" << v01 << endl; // v01:103.281

    auto v02 = bond_price_zero({100, 0.04, 3, "A", 0.04}, 41082, IC, 0.0);
    cout << "v02:" << v02 << endl; // v02:106.992
  }

  // PV01: Price value of a basis point
  // Increase 1bp in yield curve
  FixedRateBond frb1{100, 0.03, 30, "M", 0.05};
  FixedRateBond frb2{1000, 0.08, 6, "S", 0.10};
  auto v0 = bond_price_zero(frb2, 41082, IC, 0.01);
  auto v1 = bond_price_zero(frb2, 41082, IC, 0.0);
  auto v2 = bond_price_zero(frb2, 41082, IC, -0.01);
  {
    double pv01 = (v1 - v2); // / 0.0001 / 100 = per 1 bp change in %
    cout << "v1: " << v1 << ", "
         << "v2: " << v2 << " pv01: " << pv01 << endl;
  }
  {
    double pv01 = (v0 - v1);
    cout << "v0: " << v0 << ", "
         << "v1: " << v1 << " pv01: " << pv01 << endl;
  }
  {
    double pv01 = (v0 - v2) / 2;
    cout << "v0: " << v0 << ", "
         << "v2: " << v2 << " pv01: " << pv01 << endl;
  }
  cout << "====" << endl;
}
