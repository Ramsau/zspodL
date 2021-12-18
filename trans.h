#ifndef TRANS_H
#define TRANS_H

#include <RTClib.h>

namespace Trans {
  extern const char* weekdays[7];

  extern const char* hours[12];
  extern const char* minutes[5];
  extern const char* before;
  extern const char* after;
  extern const char* half;
  

  extern void upperString(DateTime dt, char* buf);
  extern void lowerString(DateTime dt, char* buf);
  extern int roundTo(int num, int stepSize);
}

#endif
