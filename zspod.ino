#include <Wire.h>
#include <RTClib.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include <SPI.h>
#include "epd4in2.h"
#include "epdpaint.h"

#include "buttons.h"
#include "trans.h"
#include "timing.h"
#include "globals.h"

#define COLORED     0
#define UNCOLORED   1

#define TOPBAR_HEIGHT 30
#define BIGSTRING_MARGIN 5
#define BIGSTRING_SINGLE_Y ((300 - TOPBAR_HEIGHT - FONT60_HEIGHT) / 2  + TOPBAR_HEIGHT)
#define BIGSTRING_DOUBLE_Y_UPPER ((300 - TOPBAR_HEIGHT - (FONT60_HEIGHT * 2) - BIGSTRING_MARGIN ) / 2 + TOPBAR_HEIGHT)
#define BIGSTRING_DOUBLE_Y_LOWER (BIGSTRING_DOUBLE_Y_UPPER + FONT60_HEIGHT + BIGSTRING_MARGIN)
 
#define SMALLSTRING_X (400 - 17 * 5) / 2
#define SMALLSTRING_Y (300 - TOPBAR_HEIGHT - 24) / 2 + TOPBAR_HEIGHT
 
#define SERIAL_DEACTIVATE


unsigned char image[750];
Paint paint(image, 200, 30);
Epd epd;

char text_1[20] = {0};
char text_2[20] = {0};

void setup() {
  // slow down the clock for power saving
  CLKPR = (1<<CLKPCE) | (1<<CLKPS1);

  // RTC and serial
  #ifdef SERIAL_ACTIVATE
  Serial.begin(9600);
  #endif
  rtc.begin();
  //rtc.adjust(DateTime(__DATE__, __TIME__));

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

  // (400 - strlen(date)* fontwidth) / 2
  int fromLeft = 200 - (strlen(date) * 17) / 2;

  epd.Init();
  epd.ClearFrameHidden();

  paint.SetHeight(TOPBAR_HEIGHT);
  paint.SetWidth(200);
  paint.Clear(UNCOLORED);
  paint.DrawFilledRectangle(0,0, 200, TOPBAR_HEIGHT, COLORED);
  paint.DrawStringAt(fromLeft, 5, date, &Font24, UNCOLORED);
  paint.DrawStringAt(100, 100, timecode, &Font24, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawFilledRectangle(0, 0, 200, TOPBAR_HEIGHT, COLORED);
  paint.DrawStringAt(fromLeft - 200, 5, date, &Font24, UNCOLORED);
  epd.SetPartialWindow(paint.GetImage(), 200, 0, paint.GetWidth(), paint.GetHeight());


  if (!time_change_mode) {
    Trans::upperString(now, text_1);
    Trans::lowerString(now, text_2);
  
    int width_upper = paint.BigStringWidth(text_1, font60);
    int width_lower = paint.BigStringWidth(text_2, font60);
  
    int from_left_upper = 200 - width_upper / 2;
    int from_left_lower = 200 - width_lower / 2;
    
    paint.DrawBigStringAt(from_left_upper, BIGSTRING_DOUBLE_Y_UPPER, text_1, font60, COLORED, epd);
    paint.DrawBigStringAt(from_left_lower, width_upper ? BIGSTRING_DOUBLE_Y_LOWER : BIGSTRING_SINGLE_Y, text_2, font60, COLORED, epd);
  }
  else {
    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 0, timecode, &Font24, COLORED);
    epd.SetPartialWindow(paint.GetImage(), SMALLSTRING_X, SMALLSTRING_Y, paint.GetWidth(), paint.GetHeight());
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
