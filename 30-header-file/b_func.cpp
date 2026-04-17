#include "a_func.h"

// b_func.h already defines b_func(), re-definition compiler error
#include "b_func.h"

int b_func() { return a_func() + 3; }