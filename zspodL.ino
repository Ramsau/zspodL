#include <Wire.h>
#include <RTClib.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <SPI.h>
#include "epd7in5_V2.h"

#include "buttons.h"
#include "trans.h"
#include "timing.h"
#include "globals.h"

#define COLORED     1
#define UNCOLORED   0

#define TOPBAR_HEIGHT 60
#define TOPBAR_PADDING_TOP 18
#define BIGSTRING_MARGIN 5
#define MAIN_FONT_HEIGHT 110
#define BIGSTRING_SINGLE_Y ((EPD_HEIGHT - TOPBAR_HEIGHT - MAIN_FONT_HEIGHT) / 2  + TOPBAR_HEIGHT)
#define BIGSTRING_DOUBLE_Y_UPPER ((EPD_HEIGHT - TOPBAR_HEIGHT - (MAIN_FONT_HEIGHT * 2) - BIGSTRING_MARGIN ) / 2 + TOPBAR_HEIGHT)
#define BIGSTRING_DOUBLE_Y_LOWER (BIGSTRING_DOUBLE_Y_UPPER + MAIN_FONT_HEIGHT + BIGSTRING_MARGIN)
 
#define SMALLSTRING_X (EPD_HEIGHT - 17 * 5) / 2
#define SMALLSTRING_Y (EPD_WIDTH - TOPBAR_HEIGHT - 24) / 2 + TOPBAR_HEIGHT
 
#define SERIAL_ACTIVATE

Epd epd;

char text_1[20] = {0};
char text_2[20] = {0};

void setup() {
  // slow down the clock for power saving
  CLKPR = (1<<CLKPCE) | (1<<CLKPS1);

  // RTC and serial
  #ifdef SERIAL_ACTIVATE
  Serial.begin(57600);
  #endif
  rtc.begin();
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  DateTime now = rtc.now();
  #ifdef SERIAL_ACTIVATE
  Serial.print("Start time: ");
  char dt_format[] = DATE_TIME_FORMAT;
  Serial.println(now.toString(dt_format));
  #endif

  // buttons
  init_buttons();

  // display
  updateDisplay();

  
  // Timing
  Timer::init_timer();
  delay(100); 
}


void loop() {
  int secs_to_sleep = time_change_mode ? 5 : secsToSleep();
  time_change_mode = false;
  Timer::start_timer(secs_to_sleep);
  Timer::do_sleep();
  updateDisplay();
}

void  updateDisplay() {

  
  DateTime now = rtc.now();
  //now = DateTime(2021, 12, 12, 11, 58, 0);

  char dateTemplate[] = ", DD.MM.YYYY";
  char date[30];
  const char* weekday = Trans::weekdays[now.dayOfTheWeek()];
  strcpy(date, weekday);
  strcpy(date + strlen(weekday), now.toString(dateTemplate));

  char timecode[6];
  char t_format[] = TIME_FORMAT;
  strcpy(timecode, now.toString(t_format));

  #ifdef SERIAL_ACTIVATE
  char dt_format[] = DATE_TIME_FORMAT;
  Serial.print("Refresh Time: ");
  Serial.println(now.toString(dt_format));
  #endif

  int fromLeft = (EPD_WIDTH - epd.BigStringWidth(date, &font40)) / 2;

  epd.Init();
  epd.ClearFrameHidden();

  epd.SetPartialWindow(0, 0, 0, EPD_WIDTH, TOPBAR_HEIGHT);
  epd.DisplayText(date, &font40, UNCOLORED, fromLeft, TOPBAR_PADDING_TOP);

  if (!time_change_mode) {
    Trans::upperString(now, text_1);
    Trans::lowerString(now, text_2);
  
    int width_upper = epd.BigStringWidth(text_1, &font110);
    int width_lower = epd.BigStringWidth(text_2, &font110);

    int from_left_upper = (EPD_WIDTH - width_upper) / 2;
    int from_left_lower = (EPD_WIDTH - width_lower) / 2;
    
    epd.DisplayText(text_1, &font110, COLORED, from_left_upper, BIGSTRING_DOUBLE_Y_UPPER);
    epd.DisplayText(text_2, &font110, COLORED, from_left_lower, width_upper ? BIGSTRING_DOUBLE_Y_LOWER : BIGSTRING_SINGLE_Y);
  }
  else {
    int from_left = (EPD_WIDTH - epd.BigStringWidth(timecode, &font110)) / 2;
    epd.DisplayText(timecode, &font110, COLORED, from_left, BIGSTRING_SINGLE_Y);
  }

  epd.DisplayFrame();
  epd.Sleep();

  // wait for flush
  delay(50);

  // clear reset pin to reserve power
  digitalWrite(8, LOW);
  pinMode(8, INPUT);
}

int secsToSleep() {
  DateTime now = rtc.now();
  //now = DateTime(2021, 12, 12, 11, 58, 0);
  
  uint8_t mins = now.minute();
  int next_wakeup_mins = Trans::roundTo(mins, 5) + 3;

  int secs_til_wake = (next_wakeup_mins - mins) * 60 - now.second() + 3;
  
  return secs_til_wake;
}
