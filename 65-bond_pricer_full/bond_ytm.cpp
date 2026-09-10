#include "bond_ytm.h"
#include "global.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

auto bond_price_ytm(FixedRateBond frb) {
  int payment_count = get_payment_count(frb.ps);
  int n = frb.maturity * payment_count;
  vector<double> cf(n, frb.coupon_rate / payment_count * frb.face_value),
      df(n, 0);
  cf.back() += frb.face_value;
  double pv = 0.0;
  double dcf = 0.0;
  double mac_duration = 0.0;
  double convexity = 0.0;
  auto unit_rate = (1.0 + frb.ytm / payment_count);
  for (int i = 0; i < n; i++) {
    df[i] = 1.0 / pow(unit_rate, i + 1);
    dcf = df[i] * cf[i];
    pv += dcf;
    mac_duration += dcf * (i + 1) / payment_count;
    convexity += dcf * (i + 2) * (i + 1);
    if (DEBUG)
      cout << i << ": " << df[i] << ", " << cf[i] << ", " << dcf << ", " << pv
           << "," << '\n';
  }
  mac_duration /= pv;
  convexity /= pv * unit_rate * unit_rate * payment_count * payment_count;
  double mod_duration = mac_duration / unit_rate;
  double dv01 = mod_duration * pv / 10000; // $ per 1bp
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

auto test_print_bond_price_ytm(FixedRateBond frb) {
  auto [pv, mac_duration, mod_duration, dv01, convexity] = bond_price_ytm(frb);
  cout << "bond: " << frb.face_value << ", coupon: " << frb.coupon_rate
       << ", maturity: " << frb.maturity << ", ps: " << frb.ps
       << ", ytm: " << frb.ytm << '\n';
  cout << "pv: " << pv << ", "
       << "mac: " << mac_duration << ", "
       << "mod: " << mod_duration << ", "
       << "dv01: " << dv01 << ", "
       << "convexity: " << convexity << '\n';
  return make_tuple(pv, mac_duration, mod_duration, dv01, convexity);
}

void test_bond_price_ytm() {
  cout << "test_bond_price_ytm: " << '\n';
  cout << setprecision(10);

  {
    cout << "Test PV" << '\n';
    auto [pv, mac_duration, mod_duration, dv01, convexity] =
        test_print_bond_price_ytm({100, 0.06, 30, "A", 0.06});
    isclose(pv, 100) ? (cout << "Test PV: PASS" << '\n'
                             << "----" << '\n')
                     : throw logic_error("Sampole bond test error");
  }

  {
    // https://www.wallstreetmojo.com/convexity-of-a-bond-formula-duration/
    cout << "Test convexity" << '\n';
    auto [pv, mac_duration, mod_duration, dv01, convexity] =
        test_print_bond_price_ytm({1000, 0.08, 6, "S", 0.10});
    (isclose(pv, 911.3674836) && isclose(mac_duration, 4.817782802) &&
     isclose(mod_duration, 4.588364574) && isclose(dv01, 0.4181686275) &&
     isclose(convexity, 26.26239568))
        ? (cout << "Test convexity: PASS" << '\n'
                << "----" << '\n')
        : throw logic_error("Sample bond test failure");
  }

  {
    //  https://en.wikipedia.org/wiki/Bond_duration#Example_2
    cout << "Test mac_duration" << '\n';
    auto [pv, mac_duration, mod_duration, dv01, convexity] =
        test_print_bond_price_ytm({1000, 0.05, 5, "A", 0.065});
    isclose(mac_duration, 4.528943201)
        ? (cout << "Test mac_duration: PASS" << '\n'
                << "----" << '\n')
        : throw logic_error("Sample bond test failure");
  }

  {
    cout << "Test modified duration/effective_duration/dv01" << '\n';
    auto [pv, mac_duration, mod_duration, dv01, convexity] =
        bond_price_ytm({100, 0.03, 30, "M", 0.05});
    cout << pv << ", " << mac_duration << ", " << mod_duration << "," << dv01
         << ", " << convexity << '\n';

    cout << "test duration: " << '\n';
    cout << "macaulay duration: " << mac_duration << '\n';
    cout << "modified duration: " << mod_duration << '\n';

    double effecitve_duration =
        (get<0>(bond_price_ytm({100, 0.03, 30, "M", 0.05 - 0.0001 / 2.0})) -
         get<0>(bond_price_ytm({100, 0.03, 30, "M", 0.05 + 0.0001 / 2.0}))) /
        pv;
    cout << "effective_duration: " << effecitve_duration << '\n';
    double mod_duration_dv01 = dv01 / pv;
    cout << "dv01: " << dv01 << ", " << effecitve_duration * pv << '\n';
    isclose(dv01, effecitve_duration * pv, -4)
        ? (cout << "Test effective_duration/dv01: PASS" << '\n'
                << "----" << '\n')
        : throw logic_error("Sample bond effective_duration/dv01 failure");

    // mod_duration in %, so / 100.
    // mod_duration_alt is on change of 1 bp, scale to 1%
    cout << "mod: " << mod_duration / 100 << ", " << mod_duration_dv01 * 100
         << ", " << effecitve_duration * 100 << '\n';
    isclose(mod_duration / 100, mod_duration_dv01 * 100)
        ? (cout << "Test mod_duration/dv01: PASS" << '\n'
                << "----" << '\n')
        : throw logic_error("Sample bond mod_duration/dv01 failure");
  }

  {
    cout << "test for modified duration and convexity" << '\n';
    FixedRateBond frb{100, 0.03, 30, "A", 0.03};
    auto [pv, mac_duration, mod_duration, dv01, convexity] =
        bond_price_ytm(frb);

    double chg = -0.002;
    auto [pv1, _a, _b, _c, _d] = bond_price_ytm([chg](auto frb) {
      frb.ytm += chg;
      return frb;
    }(frb));
    cout << "pv: " << pv1 << "-" << pv << "=" << (pv1 - pv) << '\n';
    auto approx_mod = -mod_duration * chg;
    auto approx_mod_convexity =
        -mod_duration * chg + 0.5 * convexity * chg * chg;
    auto approx_pv = approx_mod_convexity * pv;
    auto diff_percent = (approx_pv - (pv1 - pv)) / pv;
    cout << approx_mod << ", " << approx_mod_convexity << ", " << approx_pv
         << ", " << diff_percent * 100 << "%" << '\n';
    isclose(approx_pv, (pv1 - pv), -3)
        ? (cout << "Test for modified duration and convexity: PASS" << '\n'
                << "----" << '\n')
        : throw logic_error("Sample bond duration/convexity failure");
  }

  cout << "====" << '\n';
}
