#include "globals.h"
#include "timing.h"
#include <RTClib.h>
#include <Arduino.h>
#include <avr/sleep.h>

namespace Timer
{
  volatile signed int iters_till_wake = 1;
  static const int timer_1hz_pin = 3;

  void isr() {
  }
  
  void init_timer()
  {
    pinMode(timer_1hz_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(timer_1hz_pin), isr, RISING);
  }

  void stop_timer() {
    rtc.writeSqwPinMode(DS1307_OFF);
  }

  void start_timer(int seconds_till_wake) {
    iters_till_wake = seconds_till_wake;
    rtc.writeSqwPinMode(DS1307_SquareWave1HZ);
  }


  void do_sleep()
  {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    while (iters_till_wake-- > 0) {
      sleep_mode();
      // handle button press async
      if (button_pressed) {
        check_handle_button_press();
        return;
      }
    }
  
    sleep_disable();
    stop_timer();
  }
}
