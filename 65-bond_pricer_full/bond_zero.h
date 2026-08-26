#include "global.h"
#include "rate_curve.h"
#include <string>

double bond_price_zero(FixedRateBond frb, const int today, const RateCurve IC,
                       double ic_spread = 0.0);

void test_bond_price_zero();
