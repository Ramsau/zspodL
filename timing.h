#ifndef TIMING_H
#define TIMING_H

#include <RTClib.h>

namespace Timer
{
  extern volatile signed int iters_till_wake;
  
  extern void init_timer();
  extern void do_sleep();
  extern void start_timer(int seconds_till_wake);
  extern void stop_timer();
}

#endif
