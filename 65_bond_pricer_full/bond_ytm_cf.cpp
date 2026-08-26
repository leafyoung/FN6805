#include "bond_ytm_cf.h"
#include "global.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

auto cashflow_duration(const vector<double> &cf, double yield,
                       int payment_count) {
  int n = cf.size();
  vector<double> df(n, 0), dcf(n, 0), mac_duration_items(n, 0),
      convexity_items(n, 0);
  auto unit_rate = (1.0 + yield / payment_count);
  vector<int> serial(n);
  iota(serial.begin(), serial.end(), 1);

  // df
  transform(serial.begin(), serial.end(), df.begin(),
            [&unit_rate](auto i) { return 1.0 / pow(unit_rate, i); });

  /*
cout << "df: ";
for (auto v : df) {
  cout << v << ", ";
}
cout << endl;

cout << "cf: ";
for (auto v : cf) {
  cout << v << ", ";
}
cout << endl;
*/
  // dcf
  transform(df.begin(), df.end(), cf.begin(), dcf.begin(),
            [](auto df, auto cf) { return df * cf; });
  /*
  cout << "dcf: ";
  for (auto v : dcf) {
    cout << v << ", ";
  }
  cout << endl;
  */

  // pv
  auto pv = accumulate(dcf.begin(), dcf.end(), 0.0);
  // mac_duration
  transform(
      dcf.begin(), dcf.end(), serial.begin(), mac_duration_items.begin(),
      [payment_count](auto dcf, auto i) { return dcf * i / payment_count; });
  auto mac_duration =
      accumulate(mac_duration_items.begin(), mac_duration_items.end(), 0.0) /
      pv;
  // convexity
  transform(dcf.begin(), dcf.end(), serial.begin(), convexity_items.begin(),
            [payment_count](auto dcf, auto i) { return dcf * (i + 1) * i; });
  auto convexity =
      accumulate(convexity_items.begin(), convexity_items.end(), 0.0) / pv /
      unit_rate / unit_rate / payment_count / payment_count;
  ;
  double mod_duration = mac_duration / unit_rate;
  double dv01 = mod_duration * pv / 10000; // $ per 1bp
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

auto bond_price_ytm_cf(FixedRateBond frb) {
  int payment_count = get_payment_count(frb.ps);
  int n = frb.maturity * payment_count;
  vector<double> cf(n, frb.coupon_rate / payment_count * frb.face_value);
  cf.back() += frb.face_value;
  return cashflow_duration(cf, frb.ytm, payment_count);
}

auto bond_price_ytm_cf(FloatingRateBond frb) {
  int payment_count = get_payment_count(frb.ps);
  int n = frb.maturity * payment_count;
  vector<double> cf(n, 0.0);
  vector<int> serial(n);
  iota(serial.begin(), serial.end(), 1);
  auto unit_rate = (1.0 + frb.ytm / payment_count);
  transform(serial.begin(), serial.end(), cf.begin(),
            [&unit_rate, &frb, &payment_count](const auto i) {
              return (frb.ytm / payment_count + frb.spread) * frb.face_value;
            });
  if (!frb.is_loan) {
    cf.back() += frb.face_value;
  }
  return cashflow_duration(cf, frb.ytm, payment_count);
}

auto bond_price_ytm_cf(AmortizationBond ab) {
  int payment_count = get_payment_count(ab.ps);
  int n = ab.maturity * payment_count;
  auto installment = ab.principal * (ab.yield / payment_count /
                                     (1.0 - pow(1.0 + ab.yield / payment_count,
                                                -payment_count * ab.maturity)));
  vector<double> cf(n, installment);
  return cashflow_duration(cf, ab.yield, payment_count);
}

auto test_bond_price_ytm_cf(FixedRateBond frb) {
  cout << "FixedRatedBond:" << endl;
  auto [pv, mac_duration, mod_duration, dv01, convexity] =
      bond_price_ytm_cf(frb);
  cout << "bond: " << frb.face_value << ", coupon: " << frb.coupon_rate
       << ", maturity: " << frb.maturity << ", ps: " << frb.ps
       << ", ytm: " << frb.ytm << endl;
  cout << "pv: " << pv << ", "
       << "mac: " << mac_duration << ", "
       << "mod: " << mod_duration << ", "
       << "dv01: " << dv01 << ", "
       << "convexity: " << convexity << endl;
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

auto test_bond_price_ytm_cf(FloatingRateBond frb) {
  cout << "FloatingRatedBond:" << endl;
  auto [pv, mac_duration, mod_duration, dv01, convexity] =
      bond_price_ytm_cf(frb);
  cout << boolalpha;
  cout << "bond: " << frb.face_value << ", maturity: " << frb.maturity
       << ", ps: " << frb.ps << ", ytm: " << frb.ytm
       << ", is_loan: " << frb.is_loan << ", spread: " << frb.spread << endl;
  cout << "pv: " << pv << ", "
       << "mac: " << mac_duration << ", "
       << "mod: " << mod_duration << ", "
       << "dv01: " << dv01 << ", "
       << "convexity: " << convexity << endl;
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

auto test_bond_price_ytm_cf(AmortizationBond ab) {
  cout << "AmortizationBond:" << endl;
  auto [pv, mac_duration, mod_duration, dv01, convexity] =
      bond_price_ytm_cf(ab);
  auto payment_count = get_payment_count(ab.ps);
  auto installment = ab.principal * (ab.yield / payment_count /
                                     (1.0 - pow(1.0 + ab.yield / payment_count,
                                                -payment_count * ab.maturity)));
  cout << "bond: " << ab.principal << ", installment: " << installment
       << ", maturity: " << ab.maturity << ", ps: " << ab.ps
       << ", yield: " << ab.yield << endl;
  cout << "pv: " << pv << ", "
       << "mac: " << mac_duration << ", "
       << "mod: " << mod_duration << ", "
       << "dv01: " << dv01 << ", "
       << "convexity: " << convexity << endl;
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

void test_bond_price_ytm_cf() {
  cout << "test_bond_price_ytm_cf" << endl;
  test_bond_price_ytm_cf(FloatingRateBond{.face_value = 100,
                                          .maturity = 10,
                                          .ps = "S",
                                          .ytm = 0.06,
                                          .spread = 0.0,
                                          .is_loan = false});
  test_bond_price_ytm_cf(FloatingRateBond{.face_value = 100,
                                          .maturity = 20,
                                          .ps = "A",
                                          .ytm = 0.06,
                                          .spread = -0.01,
                                          .is_loan = false});

  test_bond_price_ytm_cf(FixedRateBond{.face_value = 1000,
                                       .coupon_rate = 0.02,
                                       .maturity = 10,
                                       .ps = "M",
                                       .ytm = 0.02});
  test_bond_price_ytm_cf(AmortizationBond{
      .principal = 1000.0, .maturity = 10, .ps = "M", .yield = 0.02});
  test_bond_price_ytm_cf(FloatingRateBond{.face_value = 1000.0,
                                          .maturity = 10,
                                          .ps = "M",
                                          .ytm = 0.02,
                                          .spread = 0.0,
                                          .is_loan = false});
  test_bond_price_ytm_cf(FloatingRateBond{.face_value = 1000.0,
                                          .maturity = 10,
                                          .ps = "M",
                                          .ytm = 0.1104,
                                          .spread = 0.0,
                                          .is_loan = true});

  cout << "====" << endl;
}
