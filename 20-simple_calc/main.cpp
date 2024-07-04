#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main() {
  double x{};
  double y{};
  char op{};

  string dummy;

  cout << "value x> "s;
  cin >> x;
  getline(cin, dummy);
  if (cin.fail()) {
    cout << "Unknown input for x"
         << "\n";
    return 0;
  }

  cout << "value y> "s;
  cin >> y;
  getline(cin, dummy);
  if (cin.fail()) {
    cout << "Unknown input for x"
         << "\n";
    return 0;
  }

  cout << "operator (+, -, *, /)> ";
  cin >> op;

  double result{};
  bool done{false};

  switch (op) {
  case '+':
    result = x + y;
    done = true;
    break;
  case '-':
    result = x - y;
    done = true;
    break;
  case '*':
    result = x * y;
    done = true;
    break;
  case '/':
    result = x / y;
    done = true;
    break;
  default:
    cout << "unknown operator\n";
  }

  if (done) {
    cout << "result> " << result << "\n"
         << "Done\n";
  }
}