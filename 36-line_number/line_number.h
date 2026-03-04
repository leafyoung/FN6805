#pragma once

#include <functional>
#include <vector>

void line_number_static(size_t current, std::vector<size_t>& line_numbers);
void line_number_dynamic(size_t current, std::vector<size_t>& line_numbers);    // Baseline: single loop with branching
void line_number_dynamic_opt(size_t current, std::vector<size_t>& line_numbers); // Optimized: three loops, zero branches
void line_number_dynamic_stl(size_t current, std::vector<size_t>& line_numbers); // STL: std::generate with lambdas

