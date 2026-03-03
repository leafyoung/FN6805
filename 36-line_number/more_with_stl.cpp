#include <algorithm>
#include <functional>
#include <sstream>
#include <vector>
using namespace std;

void line_number_dynamic_opt1(int current, vector<int> &line_numbers) {
  // optimized
  const auto current_1 = current - 1;
  if (current > static_cast<int>(line_numbers.size())) {
    line_numbers.resize(current);
  }
  if (current < static_cast<int>(line_numbers.size()) / 2) {
    for (int i = 0; i < static_cast<int>(line_numbers.size()); ++i) {
      if (i > current_1) {
        line_numbers[i] = i - current_1;
      } else if (i < current_1) {
        line_numbers[i] = current + 1 - i;
      } else if (i == current_1) { // probability is low: 1/n
        line_numbers[i] = (current == 1) ? 2 : current;
      } else {
        ostringstream ss;
        ss << "should never reach here: " << current;
        throw logic_error(ss.str());
      }
    }
  } else {
    for (int i = 0; i < static_cast<int>(line_numbers.size()); ++i) {
      if (i < current_1) {
        line_numbers[i] = current + 1 - i;
      } else if (i > current_1) {
        line_numbers[i] = i - current_1;
      } else if (i == current_1) { // least likely
        line_numbers[i] = (current == 1) ? 2 : current;
      } else {
        ostringstream ss;
        ss << "should never reach here: " << current;
        throw logic_error(ss.str());
      }
    }
  }
}

void line_number_dynamic_opt2(int current, vector<int> &line_numbers) {
  if (current < 1) {
    throw logic_error("current cannot be < 1");
  }
  if (current > static_cast<int>(line_numbers.size())) {
    line_numbers.resize(current);
  }
  const auto current_1 = current - 1;
  int i = 0;
  generate(line_numbers.begin(), line_numbers.begin() + current - 1,
           [current, &i]() { return current + 1 - i++; });
  line_numbers[current_1] = (current == 1) ? 2 : current;
  i = 1;
  generate(line_numbers.begin() + current, line_numbers.end(),
           [&i]() { return i++; });
}
