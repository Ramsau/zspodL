#ifndef GLOBALS_H
#define GLOBALS_H

#include <RTClib.h>

#define TIME_FORMAT "hh:mm"
#define DATE_TIME_FORMAT "DD/MM/YYYY hh:mm:ss"

extern RTC_DS1307 rtc;
extern volatile bool time_change_mode;

#endif
