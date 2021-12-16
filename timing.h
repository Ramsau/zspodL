#ifndef TIMING_H
#define TIMING_H

#include <RTClib.h>

namespace Timer
{
  extern void timer_wake_interrupt();
  extern void init_timer();
  extern signed int seconds_till_wake;
  extern void (*timed_func)();
  extern void start_timer();
  extern void stop_timer();
}

#endif
