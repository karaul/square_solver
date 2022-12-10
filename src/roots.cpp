#include <math.h>

#include "roots.h"

void f_two_roots(const float &a, const float &b, const float &c, float &x1,
                 float &x2, float &xmin) {
  float d2, d, b2 = b / 2.0;
  d2 = pow(b2, 2) - a * c;
  if (d2 >= 0) {
    d = sqrt(d2);
    x1 = (-b2 - d) / a;
    x2 = (-b2 + d) / a;
  } else {
    x1 = NAN;
    x2 = NAN;
  }
  xmin = -b / a;
};

void f_one_root(const float &b, const float &c, float &x) {
  x = (b != 0.0) ? -c / b : NAN;
};
