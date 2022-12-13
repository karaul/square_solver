#include <bitset>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

#include "roots.h"
#include "treys.h"

// define type of the trey depending on NaN and zedos distribution
treys::TreyType treys::Trey::getType(const int &mask,
                                     std::ostringstream &strey) {

  std::bitset<ntrey> f_nan(mask);
  strey << "(";
  for (int k = 0; k < ntrey; ++k) { // output "( a b c)" in strey with NaN
                                    // recognition based on trey_flag
    strey << " " << (!f_nan[k] ? std::to_string(this->nums[k]) : "NaN");
  }
  strey << ")";

  // type_trey enum variable fort switch/case below
  auto type = static_cast<TreyType>((mask << 3) |
                                    ((((this->nums[2] == 0) & !f_nan[2]) << 2) |
                                     (((this->nums[1] == 0) & !f_nan[1]) << 1) |
                                      ((this->nums[0] == 0) & !f_nan[0])));
#ifdef DEBUG
  std::cout << "type: " << type << "; " << std::endl;
#endif
  return type;
}

void treys::Trey::operator()(const int &mask) {
  // consumer_worker for trey (a, b, c) with mask
  // solves eqution a*x^2 + b*x + c = 0 and finds extermum

  std::ostringstream strey, sroot, sxmin;

  sroot << std::fixed  << std::setprecision(3) << std::scientific;
  sxmin << std::fixed  << std::setprecision(3) << std::scientific;

  // define type of the trey
  auto type = this->getType(mask, strey);

  // variables for square eqiation and extremum
  a = (float)this->nums[0], 
       b = (float)this->nums[1],
       c = (float)this->nums[2];
  auto x1 = 0.0f, x2 = 0.0f, xmin = 0.0f;

  // auxalliry lambda's for formatted output depending on the case
  auto s_two_roots = [&]() {
    f_two_roots(a, b, c, x1, x2, xmin);
    if (!std::isnan(x1)) {
      sroot << " roots: (" << x1 << ", " << x2 << ")";
    } else {
      sroot << " no roots (Discr < 0)";
    };
    sxmin << "extremum Xmin=" << xmin;
  };

  auto s_one_root = [&]() {
    f_one_root(b, c, x1);
    if (!std::isnan(x1)) {
      sroot << " roots: (" << x1 << ")";
    } else {
      sroot << " no roots ";
    };
    sxmin << "not bounded";
  };

  auto s_no_roots = [&]() {
    sroot << " no roots";
    sxmin << " no extremum";
  };

  auto s_any_roots = [&]() {
    sroot << " roots any";
    sxmin << " extremum any";
  };

  auto s_zero_root = [&]() {
    sroot << " roots: ( 0 )";
    sxmin << "extremum Xmin=0";
  };

  auto s_zero_root_noextremum = [&]() {
    sroot << " roots: ( 0 )";
    sxmin << "no extremum";
  };

  // solves square equations depending on type
  switch (type) {
  case e_ABC: {
    s_two_roots();
    break;
  }
  case e_0BC: {
    s_one_root();
    break;
  }
  case e_A0C: {
    s_two_roots();
    break;
  }
  case e_00C: {
    s_no_roots();
    break;
  }
  case e_AB0: {
    s_two_roots();
    break;
  }
  case e_0B0: {
    s_zero_root_noextremum();
    break;
  }
  case e_A00: {
    s_zero_root();
    break;
  }
  case e_000: {
    s_any_roots();
    break;
  }
  case e_xBC: {
    s_one_root();
    break;
  }
  case e_x0C: {
    s_no_roots();
    break;
  }
  case e_xB0: {
    s_zero_root_noextremum();
    break;
  }
  case e_x00: {
    s_no_roots();
    break;
  }
  case e_AxC: {
    s_two_roots();
    break;
  }
  case e_0xC: {
    s_no_roots();
    break;
  }
  case e_Ax0: {
    s_zero_root_noextremum();
    break;
  }
  case e_0x0: {
    s_any_roots();
    break;
  }
  case e_ABx: {
    s_two_roots();
    break;
  }
  case e_0Bx: {
    s_zero_root_noextremum();
    break;
  }
  case e_A0x: {
    s_zero_root();
    break;
  }
  case e_00x: {
    s_any_roots();
    break;
  }
  case e_xxC: {
    s_no_roots();
    break;
  }
  case e_xx0: {
    s_no_roots();
    break;
  }
  case e_Axx: {
    s_no_roots();
    break;
  }
  case e_0xx: {
    s_no_roots();
    break;
  }
  case e_xBx: {
    s_no_roots();
    break;
  }
  case e_x0x: {
    s_no_roots();
    break;
  }
  case e_xxx: {
    s_no_roots();
    break;
  }
  };

  // final formatted output
  std::cout << strey.str() << " => " << sroot.str() << "  " << sxmin.str()
            << std::endl;
};
