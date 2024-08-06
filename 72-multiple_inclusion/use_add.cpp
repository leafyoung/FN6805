#include "use_add.h"
#include "add.h"

int add_n(int a, int b, int n) {
  int result = a;
  for (int i = 0; i < n; ++i) {
    result = add(result, b);
  }
  return result;
}