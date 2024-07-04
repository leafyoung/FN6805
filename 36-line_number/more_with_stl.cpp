#include <algorithm>
#include <functional>
#include <sstream>
#include <vector>
using namespace std;

auto line_number_dynamic_opt1(int current, vector<int> &line_numbers) {
  // optimized
  const auto current_1 = current - 1;
  if (current > line_numbers.size()) {
    line_numbers.resize(current);
  } else {
    line_numbers.clear();
  }
  if (current < line_numbers.size() / 2) {
    for (int i = 0; i < line_numbers.size(); ++i) {
      if (i > current_1) {
        line_numbers[i] = i - current_1;
      } else if (i < current_1) {
        line_numbers[i] = current_1 - i;
      } else if (i == current_1) { // probability is low: 1/n
        line_numbers[i] = current;
      } else {
        ostringstream ss;
        ss << "should never reach here: " << current;
        throw logic_error(ss.str());
      }
    }
  } else {
    for (int i = 0; i < line_numbers.size(); ++i) {
      if (i < current_1) {
        line_numbers[i] = current_1 - i;
      } else if (i > current_1) {
        line_numbers[i] = i - current;
      } else if (i == current_1) { // least likely
        line_numbers[i] = current;
      } else {
        ostringstream ss;
        ss << "should never reach here: " << current;
        throw logic_error(ss.str());
      }
    }
  }
}

auto line_number_dynamic_opt2(int current, vector<int> &line_numbers) {
  if (current < 1) {
    throw logic_error("current cannot be < 1");
  }
  if (current > line_numbers.size()) {
    line_numbers.resize(current);
  }
  const auto current_1 = current - 1;
  int i = 0;
  generate(line_numbers.begin(), line_numbers.begin() + current - 1,
           [current_1, &i]() { return current_1 - i++; });
  line_numbers[current_1] = current;
  ++i;
  generate(line_numbers.begin() + current, line_numbers.end(),
           [current_1, &i]() { return i++ - current_1; });
}
