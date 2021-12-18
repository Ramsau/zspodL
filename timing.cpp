#include "timing.h"
#include <RTClib.h>
#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>

namespace Timer
{
  volatile signed int iters_till_wake = 1;
  
  void init_timer()
  {
    TCCR2A = 0;// set entire TCCR1A register to 0
    TCCR2B = 0;// same for TCCR1B
    TCNT2  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR2A = 255; // = (16*10^6) / (1*1024) / 255 -> 61 times per second
    // turn on CTC mode
    TCCR2B |= (1 << WGM22);

    // enable async mode
    ASSR |= (1<<AS2);
    
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
  }

  void stop_timer() {
    TCCR2B &= ~((1<<CS22) | (1<<CS21) | (1<<CS20));
  }

  void start_timer(int seconds_till_wake) {
    iters_till_wake = seconds_till_wake  * 61;
    // Set CS20 - CS22 bits for 1024 prescaler
    TCCR2B |= (1<<CS22) | (1<<CS21) | (1 << CS20);  
  }

  ISR(TIMER2_COMPA_vect) {
  }

  void do_sleep()
  {
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  
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
    power_all_enable();
    stop_timer();
  }
}
