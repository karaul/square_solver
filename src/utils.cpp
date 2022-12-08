#include <bitset>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

#include "utils.h"

/* thread producer function */
void utils::producer(int &ntasksp1, char *ctasks[],
                     ringbuffer::RingBuffer &rb) {
  int val = 0, mask = 0, ntasks = ntasksp1 - 1;
  std::vector<bool> flags(3, true);

  // lambda push_mask pushes NaNs flag encoded with int mask into rb
  // it happens every ntrey elements, and at the lates incomplete trey
  auto push_mask = [&]() {
    mask = (flags[0] << 2) | (flags[2] << 1) | flags[1];
    while (!rb.push(mask))
      ;
#ifdef DEBUG
    std::cout << "push  mask = " << mask << "; " << std::endl;
#endif
  };

  for (int i = 1; i < ntasksp1; ++i) {
    int idx_of_trey = (i % ntrey);
    // idx_of_trey = 1, the 1st element (a) is in the trey
    // idx_of_trey = 2, the 2nd element (b) is in the trey
    // idx_of_trey = 0, the 3rd element (c), i.e. the trey is full,
    bool trey_ready = (idx_of_trey == 0);
    bool trey_last = (i == ntasks);

    flags[idx_of_trey] = producer_worker(ctasks[i], val);

    while (!rb.push(val))
      ;
#ifdef DEBUG
    std::cout << "push  val = " << val << "; " << std::endl;
#endif
    if (trey_ready) {
      push_mask();
      flags.assign(ntrey, true);
    } else if (trey_last)
      push_mask();

    while (rb.readIx_.load(std::memory_order_relaxed) !=
           rb.writeIx_.load(std::memory_order_relaxed))
      ;
  };
};

/* thread consumer function */
void utils::consumer(int &ntasksp1, ringbuffer::RingBuffer &rb) {

  int val = 0, mask = 0, ntasks = ntasksp1 - 1;
  std::vector<int> vals(ntrey, 0);

  // lambda pop_mask pops NaNs flag encoded inside int mask from rb
  // it happens every ntrey elements, and at the latest incomplete trey
  // Then, consumer_worker is called
  auto pop_mask = [&]() {
    while (!rb.pop(mask))
      ;
#ifdef DEBUG
    std::cout << "pop  mask = " << mask << "; " << std::endl;
#endif
    utils::consumer_worker(vals[1], vals[2], vals[0], mask);
  };

  for (int i = 1; i < ntasksp1; ++i) {
    while (!rb.pop(val))
      ;
#ifdef DEBUG
    std::cout << "pop  val = " << val << "; " << std::endl;
#endif
    int idx_of_trey = (i % ntrey);
    bool trey_ready = (idx_of_trey == 0);
    bool trey_last = (i == ntasks);

    vals[idx_of_trey] = val;

    if (trey_ready)
      pop_mask();
    else if (trey_last)
      pop_mask();
  };
};

/* producer_worker  reads element; it is true if it's garbage, and false
 * otherwise */
bool utils::producer_worker(char *s, int &val) {
  try {
    val = std::stoi(s);
    return false;
  } catch (const std::invalid_argument &e) {
    val = 0;
    return true;
  }
};

/* consumer_worker  for trey (a, b, c) */
void utils::consumer_worker(int const &a, int const &b, int const &c,
                            int const &mask) {

  std::ostringstream strey, sroot, sxmin;
  strey << "(";
  sroot << std::fixed << std::setprecision(3);
  sxmin << std::fixed << std::setprecision(3);

  std::bitset<ntrey> f_nan(mask);
  std::vector<int> vals{a, b, c};
  for (int k = 0; k < ntrey; ++k) { // output "( a b c)" in strey with NaN
                                    // recognition based on trey_flag
    strey << " " << (!f_nan[k] ? std::to_string(vals[k]) : "NaN");
  }

  auto type_trey = static_cast<utils::TypeTrey>(
      (mask << 3) | (((c == 0 & !f_nan[2]) << 2) | ((b == 0 & !f_nan[1]) << 1) | (a == 0 & !f_nan[0])));
#ifdef DEBUG
  std::cout << "type_trey: " << type_trey << "; " << std::endl;
#endif

  auto d2 = 0.0f, d = 0.0f, b2 = b / 2.0f, x1 = 0.0f, x2 = 0.0f, xmin = 0.0f;

  // auxalliry lambda's for  particular square equatons and extrema
  auto s_no_roots = [&]() {
    sroot << " no roots";
    sxmin << " no extremum";
  };

  auto s_any_roots = [&]() {
    sroot << " roots any";
    sxmin << " extremum any";
  };

  auto s_AB0_ABx = [&]() { // c=0
    x1 = 0;
    x2 = -b / (1.0f * a);
    sroot << " roots: (" << x1 << ", " << x2 << ")";
    xmin = -b / (2.0f * a);
    sxmin << "extremum Xmin=" << xmin;
  };

  auto s_A0x_Ax0 = [&]() {
    sroot << " roots: ( 0 )";
    sxmin << "extremum Xmin=0";
  };

  auto s_xB0_0Bx_0B0 = [&]() {
    sroot << " roots: ( 0 )";
    sxmin << "no extremum";
  };

  // solves square equations depending on type_trey
  switch (type_trey) {
  case e_ABC: {
    d2 = pow(b2, 2) - a * c;
    if (d2 >= 0) {
      d = sqrt(d2);
      x1 = (-b2 - d) / a;
      x2 = (-b2 + d) / a;
      sroot << " roots: (" << x1 << ", " << x2 << ")";
    } else {
      sroot << " no roots, Discr < 0";
    }
    xmin = -b / (2.0f * a);
    sxmin << "extremum Xmin=" << xmin;
    break;
  }
  case e_0BC: { // a=0
    x1 = -b / c;
    sroot << " roots: (" << x1 << ")";
    sxmin << "not bounded";
    break;
  }
  case e_A0C: { // b=0
    d2 = -c / (1.0f * a);
    if (d2 >= 0) {
      d = sqrt(d2);
      x1 = -d;
      x2 = d;
      sroot << " roots: (" << x1 << ", " << x2 << ")";
    } else {
      sroot << " no roots, Discr < 0";
    }
    sxmin << "extremum Xmin=0";
    break;
  }
  case e_00C: { // a=b=0
    s_no_roots();
    break;
  }
  case e_AB0: { // c=0
    s_AB0_ABx();
    break;
  }
  case e_0B0: { // a=c=0
    s_xB0_0Bx_0B0();
    break;
  }
  case e_A00: { // b=c=0
    sroot << " roots: ( 0 )";
    sxmin << " extremum Xmin=0";
    break;
  }
  case e_000: { // a=b=c=0
    s_any_roots();
    break;
  }
  case e_xBC: {
    x1 = -c / (1.0f * b);
    sroot << " roots: (" << x1 << ")";
    sxmin << " no extremum";
    break;
  }
  case e_x0C: {
    s_no_roots();
    break;
  }
  case e_xB0: {
    s_xB0_0Bx_0B0();
    break;
  }
  case e_x00: {
    s_no_roots();
    break;
  }
  case e_AxC: {
    d2 = -c / (1.0f * a);
    if (d2 >= 0) {
      d = sqrt(d2);
      x1 = -d;
      x2 = d;
      sroot << " roots: (" << x1 << ", " << x2 << ")";
    } else {
      sroot << " no roots, Discr < 0";
    }
    sxmin << "extremum Xmin=0";
    break;
  }
  case e_0xC: {
    s_no_roots();
    break;
  }
  case e_Ax0: {
    s_A0x_Ax0();
    break;
  }
  case e_0x0: {
    s_any_roots();
    break;
  }
  case e_ABx: {
    s_AB0_ABx();
    break;
  }
  case e_0Bx: {
    s_xB0_0Bx_0B0();
    break;
  }
  case e_A0x: {
    s_A0x_Ax0();
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
  std::cout << strey.str() << ") => " << sroot.str() << "  " << sxmin.str() << std::endl;
}
