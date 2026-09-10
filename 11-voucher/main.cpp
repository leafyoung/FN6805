#include <iostream>
#include <vector>
using namespace std;

struct Voucher {
  int value;
  bool is_spent = false;
};

struct Household {
  long long id;
  string address;
};

using Household_ID = long long;

struct HouseholdVouchers {
  Household_ID id;
  vector<Voucher> vouchers;
  int balance;
};

struct HouseholdVouchersEx {
  Household_ID id;
  vector<Voucher> vouchers;
  int balance;
  bool issued;
  int issued_time;
  int issued_amount;
};

int main() {
  std::cout << boolalpha;

  Voucher v1{100}, v2{40};
  std::cout << v1.value << '\n';
  std::cout << v2.value << '\n';

  Household h{1234, "Jurong East"};

  HouseholdVouchers hv{1234, {{3}, {4}}, 3};
  std::cout << hv.vouchers.back().value << '\n';

  Voucher v3(v1);

  cout << v3.value << '\n';

  vector<int> a{1, 2, 3}, b{4, 5, 6};

  vector<vector<int>> aInt5 = {
      {11, 12, 13, 14}, {21, 22, 23, 24}, {31, 32, 33, 34},
      {41, 42, 43, 44}, {51, 52, 53, 54},
  };
  cout << aInt5[3][3] << '\n';
}