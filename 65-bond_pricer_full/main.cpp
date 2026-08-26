#include "bond_ytm.h"
#include "bond_ytm_cf.h"
#include "bond_zero.h"
#include "global.h"
#include "rate_curve.h"

int main() {
  test_bond_price_ytm();
  test_bond_price_ytm_cf();

  test_bond_price_zero();

  test_get_df();
}