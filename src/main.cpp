/*---------------------------------------------------------------------------*\

 Solution of the Coding Assigment for Teknavo
 Evgeny Votyakov, karaul@gmail.com
 December 2022, Nicosia, Cyprus

 The code solves Single Producer - Single Consumer (SPSC) problem with
 two parallel  threads. Ring Buffer with atomic indexes is used as storage.

 Formulation of the problem.

 There is a stream of data from stdin, recorded  into *argv[] as strings.
 Most of the data can be casted as integers, but there  might be letters, which
 must be considered as garbage.

 Producer reads data as treys (a,b,c), casts into integers where it is possible,
 and if it is not possible,  puts dummy 0. The dummy 0's must be
 distinguished from real 0's in stdin, i.e. when consumer takes (a,b,c), it
knows where were healthy data and where there were initially damaged elements.

 Treys (a,b,c) from stdin are passed to consumer, who forms a square equation
 a*x^2+b*x+c=0, solves it (called roots x1,x2 of in the code), and determines
 extremum (called Xmin), and finally makes a formatted stdout.

Example:

./square_solver 1 -2 -3 y 3 9 -6 7 8

 ( 1 -2 -3) => roots: (-1.000, 3.000)  extremum Xmin=1.000
( NaN 3 9) => no roots  no extremum
( -6 7 8) => roots: (1.877, -0.710)  extremum Xmin=0.583

fast build:
 g++ -std=c++14 -pthread *.cpp
with DEBUG
 g++ -std=c++14 -pthread -DDEBUG *.cpp

 *---------------------------------------------------------------------------*/

#include <iomanip>
#include <iostream>

#include "ringbuffer.h"
#include "trey.h"
#include "utils.h"

int main(int argc, char **argv) {

  // argc below is used in the for loops producer and consumer
  // argc-1 is equal to the number of the latest element

  // uncomment variable tic for profiling
  // auto tic = std::chrono::steady_clock::now();

  ringbuffer::RingBuffer rb(ringbuffer::nelem);

  std::thread t1(utils::producer, std::ref(argc), std::ref(argv), std::ref(rb));
  std::thread t2(utils::consumer, std::ref(argc), std::ref(rb));

  t1.join();
  t2.join();

  // uncomment toc for profiling
  // auto toc = std::chrono::steady_clock::now();
  // std::cout << argc * 1000000000 /
  // std::chrono::duration_cast<std::chrono::nanoseconds>(tic - toc)
  // .count() << " ops/s" << std::endl;

  return 0;
}
