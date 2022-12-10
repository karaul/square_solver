#include <bitset>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

#include "utils.h"

void utils::producer(int &ntasksp1, char*ctasks[], ringbuffer::RingBuffer&rb){
// thread producer function 
  
  int num = 0, mask = 0, ntasks = ntasksp1 - 1;

  // lambda push_mask pushes NaNs flag encoded with int mask into rb
  // it happens every ntrey elements, and at the lates incomplete trey
  auto push_mask = [&]() {
    while (!rb.push(mask))
      ;
#ifdef DEBUG
    std::cout << "push  mask = " << mask << "; " << std::endl;
#endif
  };

  int trey_pos = 0;
  for (int i = 1; i < ntasksp1; ++i) {
    mask = mask + (producer_worker(ctasks[i], num) << trey_pos); 
    while (!rb.push(num))
      ;
#ifdef DEBUG
    std::cout << "push  num = " << num << "; " << std::endl;
#endif
    ++trey_pos;
    bool trey_ready = (trey_pos == ntrey);
    bool trey_last = (i == ntasks);

    if (trey_ready) {
      push_mask();
      trey_pos = 0;
      mask = 0;
    } 
    else { 
      if (trey_last) {
        push_mask();
      }
    }  
    
    while (rb.readIx_.load(std::memory_order_relaxed) !=
           rb.writeIx_.load(std::memory_order_relaxed))
      ;
  };
};


void utils::consumer(int &ntasksp1, ringbuffer::RingBuffer &rb) {
// thread consumer function 

  int num = 0, mask = 0, ntasks = ntasksp1 - 1;
  std::vector<int> nums(ntrey, 0);

  // lambda pop_mask pops NaNs flag encoded inside int mask from rb
  // it happens every ntrey elements, and at the latest incomplete trey
  // Then, consumer_worker is called
  auto pop_mask = [&]() {
    while (!rb.pop(mask))
      ;
#ifdef DEBUG
    std::cout << "pop  mask = " << mask << "; " << std::endl;
#endif
    utils::consumer_worker(nums[0], nums[1], nums[2], mask);
  };

  int trey_pos = 0;
  for (int i = 1; i < ntasksp1; ++i) {
    while (!rb.pop(num))
      ;
#ifdef DEBUG
    std::cout << "pop  num = " << num << "; " << std::endl;
#endif

    nums[trey_pos] = num;
    ++trey_pos;
    bool trey_ready = (trey_pos == ntrey);
    bool trey_last = (i == ntasks);

    if (trey_ready) {
      pop_mask();
      nums.assign(ntrey, 0);
      trey_pos = 0;
    }
    else {
      if (trey_last) { 
        pop_mask();
      }
    }
  };
};
/* end thread consumer function */


bool utils::producer_worker(char *msg, int &num) {
// producer_worker  reads element; 
// it returnd  true if the element is garbage, and false otherwise 
  try {
    num = std::stoi(msg);
    return false;
  } catch (const std::invalid_argument &e) {
    num = 0;
    return true;
  }
};


void utils::consumer_worker(const int &a, 
                            const int &b, 
                            const int &c,
                            const int &mask) {
// consumer_worker for trey (a, b, c) with mask
// solves eqution a*x^2 + b*x + c = 0 and finds extermum

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

  // type_trey enum variable fort switch/case below
  auto trey_type = static_cast<utils::TreyType>(
      (mask << 3) | (((c == 0 & !f_nan[2]) << 2) | ((b == 0 & !f_nan[1]) << 1) | (a == 0 & !f_nan[0])));
#ifdef DEBUG
  std::cout << "type_trey: " << trey_type << "; " << std::endl;
#endif

  // variables for square eqiation and extremun
  auto d2 = 0.0f, d = 0.0f, b2 = b / 2.0f, x1 = 0.0f, x2 = 0.0f, xmin = 0.0f;

  // auxalliry lambda's for  particular treys
  auto s_two_roots = [&]() {
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
  };

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
  switch (trey_type) {
  case e_ABC: {
    s_two_roots();
    break;
  }
  case e_0BC: { 
    x1 = -b / c;
    sroot << " roots: (" << x1 << ")";
    sxmin << "not bounded";
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
    s_AB0_ABx();
    break;
  }
  case e_0B0: { 
    s_xB0_0Bx_0B0();
    break;
  }
  case e_A00: { 
    sroot << " roots: ( 0 )";
    sxmin << " extremum Xmin=0";
    break;
  }
  case e_000: { 
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
    s_two_roots();
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
