// utils.h                                                           -*-C++-*-
#ifndef INCLUDED_SQUARE_SOLVER_UTILS
#define INCLUDED_SQUARE_SOLVER_UTILS

#include "ringbuffer.h"
#include "treys.h"

namespace utils {

/* thread producer function */
void producer(int &ntasks, char *ctasks[], ringbuffer::RingBuffer &rb);

/* thread consumer function */
void consumer(const int &ntasks, ringbuffer::RingBuffer &rb, treys::Trey &trey);

/*------------------------------------------------------------------------*\
 producer_worker is the main worker in the producer-thread.
 It obtains char *msg from *argv[], and casts it as num,
 returning true if it is garbage, and false otherwise
\*------------------------------------------------------------------------*/
bool producer_worker(char *msg, int &num);

} // namespace utils

#endif
