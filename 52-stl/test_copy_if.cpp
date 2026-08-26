#include <algorithm>
#include <iostream>
#include <vector>
// using namespace placeholders;
using namespace std;

struct StandardOption {
  double strike;
  int daysToExpiration;
};

bool is_expiring(const StandardOption &opt) {
  return opt.daysToExpiration < 10;
}

void test_copy_if() {
  cout << "\n\n====" << __FUNCTION__ << "====\n";
  StandardOption opt{93.5, 10};

  vector<StandardOption> opts{{93.5, 20}, {94.5, 9}};

  cout << opt.daysToExpiration << endl;

  for (const auto v : opts) {
    cout << v.strike << "@" << v.daysToExpiration << endl;
  }

  vector<StandardOption> result;
  copy_if(opts.begin(), opts.end(), back_inserter(result), is_expiring);

  copy_if(opts.begin(), opts.end(), back_inserter(result),
          [](auto opt) { return opt.daysToExpiration < 10; });

  cout << "Expiring:" << endl;
  for (const auto v : result) {
    cout << v.strike << "@" << v.daysToExpiration << endl;
  }
}