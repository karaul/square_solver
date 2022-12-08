// utils.h                                                           -*-C++-*-
#ifndef INCLUDED_SQUARE_SOLVER_UTILS
#define INCLUDED_SQUARE_SOLVER_UTILS

#include <bitset>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

#include "trey.h"
#include "ringbuffer.h"

namespace utils{

/* thread producer function */
void producer(int &ntasks, char *ctasks[], ringbuffer::RingBuffer &rb);

/* thread consumer function */
void consumer(int &ntasks, ringbuffer::RingBuffer &rb);

/*------------------------------------------------------------------------*\
 producer_worker is the main worker in the producer-thread.
 It obtains char *msg from *argv[], and casts it as num,
 returning true if it is garbage, and false otherwise
\*------------------------------------------------------------------------*/
bool producer_worker(char *msg, int &num);

/*------------------------------------------------------------------------*\
 consumer_worker is the main worker in the consumer-thread.
 It solves square equation a*x^2+b*x+c=0 and and find extremum
 a,b,c are elements of the trey, mask contains flags of the trey;
\*------------------------------------------------------------------------*/
void consumer_worker(int const &a, int const &b, int const &c, int const &mask);

// TreyType contains  all possible combinations of zeros and NaN's in the trey
// 0 at the position means 0;
// x at the position means NaN
enum TreyType {
  /*  no NaN's */
  e_ABC = 0,  // no zeros, no NaNs
  e_0BC = 1,  // a=0
  e_A0C = 2,  // b=0
  e_00C = 3,  // a=b=0
  e_AB0 = 4,  // c=0
  e_0B0 = 5,  // a=c=0
  e_A00 = 6,  // b=c=0
  e_000 = 7,  // a=b=c=0
              /*  a = NaN */
  e_xBC = 8,  // 8 + 0, no zeros
  e_x0C = 10, // 8 + 2, b=0
  e_xB0 = 12, // 8 + 4, c=0
  e_x00 = 14, // 8 + 2 + 4,  b=c=0
              /* b = NaN */
  e_AxC = 16, // 16 + 0, no zeros
  e_0xC = 17, // 16 + 1, a=0
  e_Ax0 = 20, // 16 + 4, c=0
  e_0x0 = 21, // 16 + 1 + 4,  a=c=0
              /* c = NaN */
  e_ABx = 32, // 32 + 0, no zeroe
  e_0Bx = 33, // 32 + 1, a=0
  e_A0x = 34, // 32 + 2, c=0
  e_00x = 38, //  32 + 2 + 4  a=c=0
              /* a = b = NaN */
  e_xxC = 24, // 8 + 16, no zeros
  e_xx0 = 28, // 8 + 16 + 4, c=0
              /* b = c = NaN */
  e_Axx = 48, // 16 + 32, no zeros
  e_0xx = 49, // 16 + 32 + 1, a=0
              /*  a = c = NaN */
  e_xBx = 40, // 8 + 32, no zeros
  e_x0x = 41, // 8 + 32 + 1, b=0
              // a = b = c = NaN
  e_xxx = 56  // 8 + 16 + 32, no zeros
};
} // namespace utils

#endif
