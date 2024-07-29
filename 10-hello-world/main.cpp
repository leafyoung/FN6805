// https://replit.com/@YeKunlun/10-hello-world

#include <iostream> // <1>
#include <string>
using namespace std; // <2>

const auto GREETING{"Hello World"s}; // <3>

void hello_world(); // <4>

int main() {     // <5>
  hello_world(); // <6>
  return 0;      // <7>
}
void hello_world() {                         // <5>
  auto name{"Alice!"s};                      // <8>
  auto greeting{GREETING};                   // <8>
  greeting += " from"s + " C++"s;            // <9>
  cout << greeting << ", " << name << "!\n"; // <9>
}