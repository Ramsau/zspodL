
#include <Arduino.h> 
#include <RTClib.h>
#include "timing.h"
#include "buttons.h"
#include "globals.h"


#define BTN_ISR_PIN 2

#define BTN_PIN_HR_UP A0
#define BTN_PIN_HR_DN A1
#define BTN_PIN_MIN_UP A2
#define BTN_PIN_MIN_DN A3
#define BTN_PIN_DAY_UP A6
#define BTN_PIN_DAY_DN A7
#define BTN_PIN_MON_UP 3
#define BTN_PIN_MON_DN 4
#define BTN_PIN_YEAR_UP 5
#define BTN_PIN_YEAR_DN 6

#define BTN_CODE_HR_UP 1
#define BTN_CODE_HR_DN 2
#define BTN_CODE_MIN_UP 3
#define BTN_CODE_MIN_DN 4
#define BTN_CODE_DAY_UP 5
#define BTN_CODE_DAY_DN 6
#define BTN_CODE_MON_UP 7
#define BTN_CODE_MON_DN 8
#define BTN_CODE_YEAR_UP 9
#define BTN_CODE_YEAR_DN 10

volatile int button_pressed = 0;

void buttonInterrupt() {
  button_pressed = 0;
  if (digitalRead(BTN_PIN_HR_UP) == HIGH) {
    button_pressed = BTN_CODE_HR_UP;
  } else if (digitalRead(BTN_PIN_HR_DN) == HIGH) {
    button_pressed = BTN_CODE_HR_DN;
  } else if (digitalRead(BTN_PIN_MIN_UP) == HIGH) {
    button_pressed = BTN_CODE_MIN_UP;
  } else if (digitalRead(BTN_PIN_MIN_DN) == HIGH) {
    button_pressed = BTN_CODE_MIN_DN;
  } else if (analogRead(BTN_PIN_DAY_UP) > 512) {  // ran out of digital pins
    button_pressed = BTN_CODE_DAY_UP;
  } else if (analogRead(BTN_PIN_DAY_DN) > 512) {
    button_pressed = BTN_CODE_DAY_DN;
  } else if (digitalRead(BTN_PIN_MON_UP) == HIGH) {
    button_pressed = BTN_CODE_MON_UP;
  } else if (digitalRead(BTN_PIN_MON_DN) == HIGH) {
    button_pressed = BTN_CODE_MON_DN;
  } else if (digitalRead(BTN_PIN_YEAR_UP) == HIGH) {
    button_pressed = BTN_CODE_YEAR_UP;
  } else if (digitalRead(BTN_PIN_YEAR_DN) == HIGH) {
    button_pressed = BTN_CODE_YEAR_DN;
  }
  time_change_mode = true;
}

void init_buttons() {
  pinMode(BTN_ISR_PIN, INPUT);
  pinMode(BTN_PIN_HR_UP, INPUT);
  pinMode(BTN_PIN_HR_DN, INPUT);
  pinMode(BTN_PIN_MIN_UP, INPUT);
  pinMode(BTN_PIN_MIN_DN, INPUT);
  pinMode(BTN_PIN_DAY_UP, INPUT);
  pinMode(BTN_PIN_DAY_DN, INPUT);
  pinMode(BTN_PIN_MON_UP, INPUT);
  pinMode(BTN_PIN_MON_DN, INPUT);
  pinMode(BTN_PIN_YEAR_UP, INPUT);
  pinMode(BTN_PIN_YEAR_DN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BTN_ISR_PIN), buttonInterrupt, RISING);
}

void check_handle_button_press() {
  if (!button_pressed) return;
  DateTime time = rtc.now();
  switch (button_pressed) {
    case BTN_CODE_HR_UP:
      time = time + TimeSpan(0, 1, 0, 0);
      break;
    case BTN_CODE_HR_DN:
      time = time + TimeSpan(0, -1, 0, 0);
      break;
    case BTN_CODE_MIN_UP:
      time = time + TimeSpan(0, 0, 1, 0);
      break;
    case BTN_CODE_MIN_DN:
      time = time + TimeSpan(0, 0, -1, 0);
      break;
    case BTN_CODE_DAY_UP:
      time = time + TimeSpan(1, 0, 0, 0);
      break;
    case BTN_CODE_DAY_DN:
      time = time + TimeSpan(-1, 0, 0, 0);
      break;
    case BTN_CODE_MON_UP:
      time = time + TimeSpan(30, 0, 0, 0);
      break;
    case BTN_CODE_MON_DN:
      time = time + TimeSpan(-30, 0, 0, 0);
      break;
    case BTN_CODE_YEAR_UP:
      time = time + TimeSpan(365, 0, 0, 0);
      break;
    case BTN_CODE_YEAR_DN:
      time = time + TimeSpan(-365, 0, 0, 0);
      break;
  }
  button_pressed = 0;

  // adjust time, wait for one second to display new time
  Serial.print("New Time: ");
  char format[] = DATE_TIME_FORMAT;
  Serial.println(time.toString(format));
  rtc.adjust(time);
  rtc.begin();
  time_change_mode=true;
  Timer::stop_timer();
  Timer::start_timer(0);
  delay(50);
}
