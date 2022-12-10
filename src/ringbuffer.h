// ringbuffer.h                                                      -*-C++-*-
#ifndef INCLUDED_SQUARE_SOLVER_RINGBUFFER
#define INCLUDED_SQUARE_SOLVER_RINGBUFFER

#include <atomic>
#include <thread>
#include <vector>

namespace ringbuffer {

// nelem is the number of elements in RingBuffer
constexpr int nelem = 19;

// ==================
//  struct RingBuffer
// ==================
struct RingBuffer {

  /*---------------------------------------------------------------------------*\
  Ring(circular) biffer is preallocated storage of size nelem cells.
  The buffer has two indexes readIx and writeIx (pointers)  to read and write
  into the cells. When readIx = writeIx, the buffer is empty independently of
  where readIx, writeIx are located.

  By writing a new element into the buffer, writeIx = writeIx+1. When writeIx is
  at the end of the buffer, it jumps to the start again. After reading a
  previously stored element located at readIx, readIx = readIx + 1. When readIx
  is at the end of the buffer, it jumps to the start. These might be imagined
  like two frogs jumping in circus, the first frog is producer, and the second
  one is consumer.

  The ring buffer allows lock-free parallel writing/reading threads in the same
  buffer by means of atomic operations to prohibit simultaneous write/read with
  the same cell.
  \*---------------------------------------------------------------------------*/

  // keeps data
  std::vector<int> data_{};

  // readIx: index to read data from data, see 64 bit alignment
  alignas(64) std::atomic<size_t> readIx_{0};

  // writeIx: index to read data from data, see 64 bit alignment
  alignas(64) std::atomic<size_t> writeIx_{0};

  // RingBuffer is empty at readIx == writeIx

  // default constructor
  RingBuffer(size_t capacity) : data_(capacity, 0){};

  // write val into data at location given writeIx
  bool push(int val);

  // read val from data at location given readIx
  bool pop(int &val);

  // true when the buffer is empty
  bool isempty();
};

} // namespace ringbuffer

#endif
