#include "globals.h"


RTC_DS1307 rtc;
DateTime now;
volatile bool time_change_mode = false;
