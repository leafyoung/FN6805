#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
// using namespace placeholders;
using namespace std;

struct StandardOption {
  double strike;
  int days_to_expiration;
};

bool is_expiring(const StandardOption &opt) {
  return opt.days_to_expiration < 10;
}

void test_copy_if() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";
  StandardOption opt{93.5, 10};

  vector<StandardOption> opts{{93.5, 20}, {94.5, 9}};

  cout << opt.days_to_expiration << '\n';

  for (const auto v : opts) {
    cout << v.strike << "@" << v.days_to_expiration << '\n';
  }

  vector<StandardOption> result;
  copy_if(opts.begin(), opts.end(), back_inserter(result), is_expiring);

  result.clear(); // same filter again, written as a lambda
  copy_if(opts.begin(), opts.end(), back_inserter(result),
          [](auto opt) { return opt.days_to_expiration < 10; });

  cout << "Expiring:" << '\n';
  for (const auto v : result) {
    cout << v.strike << "@" << v.days_to_expiration << '\n';
  }
}