
#include <Arduino.h> 
#include "buttons.h"

typedef struct{
  void(*func)();
  int pin;
} isr_pin;

void hour10_up() {
  Serial.println("hour10_up");
}

void hour10_down() {
  Serial.println("hour10_down");
}

void hour1_up() {
  Serial.println("hour1_up");
}

void hour1_down() {
  Serial.println("hour1_down");
}

void handle_buttons1() {
  Serial.println("oida");
  if (digitalRead(3) == LOW) {
    hour10_down();
  } else {
    hour10_up();
  }
}

isr_pin isr_pins[] = 
{
  {handle_buttons1, 2},
  {handle_buttons1, 3},
};

void init_buttons() {
  for (int i = 0; i < sizeof(isr_pins) / sizeof(isr_pin); i++) {
    pinMode(isr_pins[i].pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(isr_pins[i].pin), isr_pins[i].func, FALLING); 
  }
}
