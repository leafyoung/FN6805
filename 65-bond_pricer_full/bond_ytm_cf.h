#pragma once
#include <string>
#include <vector>
using namespace std;

#include "global.h"
#include "global_more.h"

auto cashflow_duration(const vector<double> &cf, double yield,
                       int payment_count);

auto bond_price_ytm_cf(FixedRateBond frb);
auto bond_price_ytm_cf(FloatingRateBond frb);
auto bond_price_ytm_cf(AmortizationBond frb);

auto test_bond_price_ytm_cf(FixedRateBond frb);
auto test_bond_price_ytm_cf(FloatingRateBond frb);
auto test_bond_price_ytm_cf(AmortizationBond frb);

void test_bond_price_ytm_cf();
