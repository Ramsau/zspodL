#include <Wire.h>
#include <RTClib.h>

#include <SPI.h>
#include "epd4in2.h"
#include "epdpaint.h"

#include "buttons.h"
#include "trans.h"
#include "timing.h"

#define COLORED     0
#define UNCOLORED   1

RTC_DS1307 rtc;

DateTime now;

unsigned char image[750];
Paint paint(image, 200, 30);
Epd epd;

void setup() {
  // RTC and serial
  Serial.begin(9600);
  rtc.begin();
  rtc.adjust(DateTime(__DATE__, __TIME__));

  now = rtc.now();
  Serial.print(" Start time: ");
  char timeStamp[] = "DD/MM/YYYY hh:mm:ss";
  Serial.println(now.toString(timeStamp));

  // buttons
  init_buttons();

  // display
  //updateDisplay();

  // disable ADC
  ADCSRA &= ~(1<<ADEN);

  Timer::timed_func = on_wakeup;
  Timer::init_timer();
  Timer::seconds_till_wake=2;
  Timer::start_timer();
}


void loop() {
}


void on_wakeup() {
  Timer::seconds_till_wake = 10;
  updateDisplay();
  Serial.println("aaa");
  Timer::start_timer();
}

void  updateDisplay() {
  
  now = rtc.now();

  char dateTemplate[] = ", DD.MM.YYYY";
  char date[30];
  const char* weekday = weekdays[now.dayOfTheWeek()];
  strcpy(date, weekday);
  strcpy(date + strlen(weekday), now.toString(dateTemplate));

  char timeTemplate[] = "hh:mm";
  char timecode[6];
  strcpy(timecode, now.toString(timeTemplate));

  // (400 - strlen(date)* fontwidth) / 2
  int fromLeft = 200 - (strlen(date) * 17) / 2;

  epd.Init();
  epd.ClearFrameHidden();
  Serial.println(timecode);

  paint.Clear(UNCOLORED);
  paint.DrawFilledRectangle(0,0, 200, 50, COLORED);
  paint.DrawStringAt(fromLeft, 3, date, &Font24, UNCOLORED);
  paint.DrawStringAt(100, 100, timecode, &Font24, COLORED);
  epd.SetPartialWindow(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  
  paint.Clear(UNCOLORED);
  paint.DrawFilledRectangle(0, 0, 200, 30, COLORED);
  paint.DrawStringAt(fromLeft - 200, 3, date, &Font24, UNCOLORED);
  epd.SetPartialWindow(paint.GetImage(), 200, 0, paint.GetWidth(), paint.GetHeight());

  
  paint.DrawBigStringAt(0, 50, "god loves", font60, COLORED, epd);
  paint.DrawBigStringAt(0, 110, "you", font60, COLORED, epd);

  epd.DisplayFrame();
  epd.Sleep();
}
