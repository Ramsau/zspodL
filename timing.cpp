#include "timing.h"
#include <RTClib.h>
#include <Arduino.h>

namespace Timer
{
  signed int seconds_till_wake = 0;
  void (*timed_func)() = 0;

  void init_timer()
  {
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR1A = 62500;// = (16*10^6) / (1*1024) * 4 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
  }

  void stop_timer() {
    TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
  }

  void start_timer() {
    // Set CS10 and CS12 bits for 1024 prescaler
    TCCR1B |= (1<<CS12) | (1 << CS10);  
  }

  ISR(TIMER1_COMPA_vect) {
    Serial.println("some ISR");
      timer_wake_interrupt();
  }

  void timer_wake_interrupt()
  {
    if (--seconds_till_wake <= 0) {
      stop_timer();
      timed_func();
    }
  }
}
