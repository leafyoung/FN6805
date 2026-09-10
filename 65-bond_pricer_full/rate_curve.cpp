#include "rate_curve.h"
#include "global.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

double _interpolate_inc(const vector<int> &xs, const vector<double> &ys,
                        const int x0, const double ic_spread) {
  xs.size() != ys.size() ? throw logic_error("xs and ys must have same size")
                         : 0;

  // use for
  if (false) {
    for (int i = 0; i < xs.size() - 1; i++) {
      if (xs[i] <= x0 && x0 <= xs[i + 1]) {
        return ys[i] + ic_spread +
               (ys[i + 1] - ys[i]) / (xs[i + 1] - xs[i]) * (x0 - xs[i]);
      }
    }
  }

  // use find_if
  auto it = find_if(xs.begin(), xs.end(), [x0](auto x) { return x0 <= x; });
  int i = it - xs.begin() - 1;
  return ys[i] + ic_spread +
         (ys[i + 1] - ys[i]) / (xs[i + 1] - xs[i]) * (x0 - xs[i]);
}

double interpolate(const vector<int> &xs, const vector<double> &ys,
                   const int x0, const double ic_spread) {
  if (x0 < xs.front()) {
    return ys.front() + ic_spread;
  } else if (x0 > xs.back()) {
    return ys.back() + ic_spread;
  } else {
    return _interpolate_inc(xs, ys, x0, ic_spread);
  }
}

double get_df(const int start, const int end, const RateCurve &ic,
              const int today, const double ic_spread) {
  start <= end ? 0 : throw logic_error("start must be less than end");

  vector<int> xs(ic.size());
  vector<double> ys(ic.size());
  // transform(ic.begin(), ic.end(), xs.begin(), [](auto p) { return get<0>(p);
  // }); transform(ic.begin(), ic.end(), ys.begin(), [](auto p) { return
  // get<1>(p); });

  for_each(ic.begin(), ic.end(), [&xs, &ys](auto entry) {
    auto [d, r] = entry;
    xs.push_back(d);
    ys.push_back(r);
  });

  double r0 = interpolate(xs, ys, start, ic_spread);
  double r1 = interpolate(xs, ys, end, ic_spread);
  if (DEBUG)
    cout << start << ", " << end << ", " << r0 << ", " << r1 << '\n';
  double d0 = exp(-(start - today) / 365.0 * r0 / 100.0);
  double d1 = exp(-(end - today) / 365.0 * r1 / 100.0);

  return d1 / d0;
}

void test_get_df() {
  cout << "test_get_df: " << '\n';

  auto today = 41082;
  cout << get_df(41082, 41083, IC, today, 0.0) << '\n';
  cout << get_df(41082, 41461, IC, today, 0.0) << '\n';
  cout << get_df(41082, 52043, IC, today, 0.0) << '\n';
  cout << get_df(41082, 52143, IC, today, 0.0) << '\n';

  auto result = get_df(41082, 44000, IC, today, 0.0);
  cout << result << '\n';
  tuple<int, double> v1{43643, 2.53599931753348}, v2{44007, 2.70067752825491};

  double r_interp = (get<1>(v2) - get<1>(v1)) / (get<0>(v2) - get<0>(v1)) *
                        (44000 - get<0>(v1)) +
                    get<1>(v1);
  double d_interp = exp(-(44000 - get<0>(IC[0])) / 365.0 * r_interp / 100.0);
  cout << d_interp << ", " << r_interp << '\n';
  isclose(d_interp, result)
      ? 0
      : throw logic_error("d_interp, result shall be close");
}
