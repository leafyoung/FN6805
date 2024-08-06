// https://replit.com/@YeKunlun/64-bondpricer?v=1

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>
using namespace std;

bool isclose(double a, double b, int power = -7) {
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

struct FixedRateBond {
  double face_value;
  double coupon_rate;
  double maturity;
  string ps;
  double ytm;
};

auto bond_price_ytm(FixedRateBond frb) {
  // mac_duration: weighted sum of
  // modified duration: % change in pv to yield change in 1%, based on
  // mac_duration yield.

  int payment_count = get_payment_count(frb.ps);
  int n = frb.maturity * payment_count;
  vector<double> cf(n, frb.coupon_rate / payment_count * frb.face_value),
      df(n, 0);
  cf.back() += frb.face_value;
  double pv = 0.0, dcf = 0.0, mac_duration = 0.0, convexity = 0.0,
         unit_rate = (1.0 + frb.ytm / payment_count);
  for (int i = 0; i < n; ++i) {
    df[i] = 1.0 / pow(unit_rate, i + 1);
    dcf = df[i] * cf[i];
    pv += dcf;
    mac_duration += dcf * (i + 1) / payment_count;
    convexity += dcf * (i + 2) * (i + 1);
  }
  mac_duration /= pv;
  convexity /= pv * pow(unit_rate, 2) * pow(payment_count, 2);
  double mod_duration = mac_duration / unit_rate;
  return make_tuple(pv, mac_duration, mod_duration, convexity);
}

auto test_print_bond_price_ytm(FixedRateBond frb) {
  auto [pv, mac_duration, mod_duration, convexity] = bond_price_ytm(frb);
  cout << "bond: " << frb.face_value << ", coupon: " << frb.coupon_rate
       << ", maturity: " << frb.maturity << ", ps: " << frb.ps
       << ", ytm: " << frb.ytm << "\n";
  cout << "pv: " << pv << ", "
       << "mac: " << mac_duration << ", "
       << "mod: " << mod_duration << ", "
       << "convexity: " << convexity << "\n";
  return make_tuple(pv, mac_duration, mod_duration, convexity);
}

void test_bond_price_ytm() {
  cout << "test_bond_price_ytm: \n";
  cout << setprecision(10);

  {
    cout << "Test PV\n";
    auto [pv, mac_duration, mod_duration, convexity] =
        test_print_bond_price_ytm({100, 0.06, 30, "A", 0.06});
    isclose(pv, 100) ? ([]() { cout << "Test PV: PASS\n----\n"; }())
                     : throw logic_error("Sample bond test error");
  }

  {
    // https://www.wallstreetmojo.com/convexity-of-a-bond-formula-duration/
    cout << "Test convexity\n";
    auto [pv, mac_duration, mod_duration, convexity] =
        test_print_bond_price_ytm({1000, 0.08, 6, "S", 0.10});
    (isclose(pv, 911.3674836) && isclose(mac_duration, 4.817782802) &&
     isclose(mod_duration, 4.588364574) && isclose(convexity, 26.26239568))
        ? ([]() { cout << "Test convexity: PASS\n----\n"; }())
        : throw logic_error("Sample bond test failure");
  }

  {
    //  https://en.wikipedia.org/wiki/Bond_duration#Example_2
    cout << "Test mac_duration\n";
    auto [pv, mac_duration, mod_duration, convexity] =
        test_print_bond_price_ytm({1000, 0.05, 5, "A", 0.065});
    isclose(mac_duration, 4.528943201)
        ? ([]() { cout << "Test mac_duration: PASS\n----\n"; }())
        : throw logic_error("Sample bond test failure");
  }

  {
    cout << "Test for modified duration and convexity\n";
    FixedRateBond frb{100, 0.02, 5, "A", 0.02};
    auto [pv, mac_duration, mod_duration, convexity] =
        test_print_bond_price_ytm(frb);

    double chg = 0.025;
    auto [pv1, _a, _b, _c] = bond_price_ytm([chg](auto frb) {
      frb.ytm += chg;
      return frb;
    }(frb));
    cout << "pv_diff: " << pv1 << "-" << pv << "=" << (pv1 - pv) << "\n";

    auto approx_mod = -mod_duration * chg;
    auto approx_mod_convexity =
        -mod_duration * chg + 0.5 * convexity * chg * chg;
    auto approx_pv = approx_mod_convexity * pv;
    auto diff_percent = (approx_pv - (pv1 - pv)) / pv;
    cout << fixed;
    cout << "approx_mod: " << approx_mod
         << ", approx_mod_convexity:" << approx_mod_convexity
         << ", approx_pv: " << approx_pv << ", diff: " << diff_percent * 100
         << "%\n";
    // less than 1%.
    isclose(approx_pv / pv, (pv1 / pv - 1), -1)
        ? ([]() {
            cout << "Test for modified duration and convexity: PASS\n----\n";
          }())
        : throw logic_error("Sample bond duration/convexity failure");
    cout << defaultfloat;
  }
  {
    cout << "test for MBS simulated\n";
    FixedRateBond frb{100, 0.02, 5, "M", 0.02};
    auto pv = get<0>(test_print_bond_price_ytm(frb));

    // interest goes down, increase possibility prepayment, so we shorten the
    // bond maturity and also yield
    auto pv_down_mbs = get<0>(test_print_bond_price_ytm([](auto frb) {
      frb.ytm += -0.01;
      frb.maturity -= 4;
      return frb;
    }(frb)));
    cout << "diff_pv_down_mbs: " << (pv_down_mbs - pv) << "\n";

    // For a FRB, it only gets smaller yield, same maturity.
    auto pv_down_frb = get<0>(test_print_bond_price_ytm([](auto frb) {
      frb.ytm += -0.01;
      return frb;
    }(frb)));
    cout << "diff_pv_down_frb: " << (pv_down_frb - pv) << "\n";

    // interest goes up, no prepayment, so we extend the bond maturity and also
    // yield
    auto pv_up_mbs = get<0>(test_print_bond_price_ytm([](auto frb) {
      frb.ytm += 0.01;
      frb.maturity += 5;
      return frb;
    }(frb)));
    cout << "diff_pv_up_mbs: " << (pv_up_mbs - pv) << "\n";

    // For FRB, it only gets higher yield, same maturity
    auto pv_up_frb = get<0>(test_print_bond_price_ytm([](auto frb) {
      frb.ytm += 0.01;
      return frb;
    }(frb)));
    cout << "diff_pv_up_frb: " << (pv_up_frb - pv) << "\n";

    (pv_up_mbs < pv_up_frb) && (pv_down_mbs < pv_down_frb)
        ? ([]() { cout << "MBS has a negative convexity.\n"; }())
        : throw logic_error("Failed test for MBS convexity.");
  }

  cout << "====\n";
}

int main() { test_bond_price_ytm(); }