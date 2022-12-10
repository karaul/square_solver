#include <bitset>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <vector>

#include "utils.h"

// thread producer function
void utils::producer(int &ntasksp1, char *ctasks[],
                     ringbuffer::RingBuffer &rb) {

  int num = 0, mask = 0, ntasks = ntasksp1 - 1;
  int trey_pos = 0;
  for (int i = 1; i < ntasksp1; ++i) {
    mask = mask + (producer_worker(ctasks[i], num) << trey_pos);
    while (!rb.push(num))
      ;
    ++trey_pos;
    bool trey_ready = (trey_pos == ntrey);
    bool trey_last = (i == ntasks);

    if (trey_ready) {
      while (!rb.push(mask))
        ;
      trey_pos = 0;
      mask = 0;
    } else {
      if (trey_last) {
        while (!rb.push(mask))
          ;
      }
    }

    while (!rb.isempty())
      ;
  }
};

// thread consumer function
void utils::consumer(const int &ntasksp1, ringbuffer::RingBuffer &rb,
                     treys::Trey &trey) {

  int ntasks = ntasksp1 - 1;

  int trey_pos = 0;
  for (int i = 1; i < ntasksp1; ++i) {
    while (!rb.pop(trey.nums[trey_pos]))
      ;

    // trey.nums[trey_pos] = num;
    ++trey_pos;
    bool trey_ready = (trey_pos == ntrey);
    bool trey_last = (i == ntasks);

    if (trey_ready) {
      while (!rb.pop(trey.mask))
        ;
      trey.worker();
      trey.nums.assign(ntrey, 0);
      trey_pos = 0;
    } else {
      if (trey_last) {
        while (!rb.pop(trey.mask))
          ;
        trey.worker();
      }
    }
  };
};

// producer_worker  reads element;
// it returns  true if the element is garbage, and false otherwise
bool utils::producer_worker(char *msg, int &num) {
  try {
    num = std::stoi(msg);
    return false;
  } catch (const std::invalid_argument &e) {
    num = 0;
    return true;
  }
};

//  consumer_worker is defined as Trey.worker() in the treys.h
