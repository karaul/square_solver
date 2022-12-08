#include <atomic>
#include <thread>
#include <vector>

#include "ringbuffer.h"

bool ringbuffer::RingBuffer::push(int val) {
// write val into data at location given writeIx
  auto const writeIx = writeIx_.load(std::memory_order_relaxed);
  auto writeIxNext = writeIx + 1;
  if (writeIxNext == data_.size()) {
    writeIxNext = 0; // jump to the start
  }
  if (writeIxNext == readIx_.load(std::memory_order_acquire)) {
    return false;
  }
  // write val at writeIx
  data_[writeIx] = val;
  // store writeIx in memory at the exact position aligned with 64 bit
  writeIx_.store(writeIxNext, std::memory_order_release);
  return true;
}

bool ringbuffer::RingBuffer::pop(int &val) {
// read val from data at location given readIx
  auto const readIx = readIx_.load(std::memory_order_relaxed);
  if (readIx == writeIx_.load(std::memory_order_acquire)) {
    return false;
  }
  // read val at readIx from data
  val = data_[readIx];
  auto readIxNext = readIx + 1;
  if (readIxNext == data_.size()) {
    readIxNext = 0; // jump to the start
  }
  // store readIx in memory at the exact position aligned with 64 bit
  readIx_.store(readIxNext, std::memory_order_release);

  return true;
}
