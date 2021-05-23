#include "button.h"

#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>

bool volatile isBtnLow;
bool volatile isBtn;

void setupBtn() {
  pinMode(BTN_PIN, INPUT_PULLUP);

  isBtnLow = false;
  isBtn = false;

  PCICR = _BV(PCIE2); // pin change interrupt enable 2 for PD0-PD7

  PCMSK2 = _BV(PCINT23); // pin change enable for PD7
}

bool getBtn() {
  bool wasBtn = isBtn;

  isBtn = false;

  return wasBtn;
}

ISR(PCINT2_vect) {
  if (digitalRead(BTN_PIN) == LOW) {
    isBtnLow = true;
  }
}
