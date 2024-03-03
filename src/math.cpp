#include "include/math.h"

int64_t Max(int64_t a, int64_t b) { return a >= b ? a : b; }

int64_t Min(int64_t am, int64_t bm) { return am <= bm ? am : bm; }

int64_t Abs(int64_t a) { return a >= 0 ? a : -1 * a; }
