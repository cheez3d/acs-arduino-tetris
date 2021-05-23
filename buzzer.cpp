#include "buzzer.h"

#include <Arduino.h>

uint8_t buzTicks = 0;

void setupBuz() {
  pinMode(BUZ_OUT, OUTPUT);

  digitalWrite(BUZ_OUT, LOW);
}

void setBuz(uint8_t ticks) {
  buzTicks = ticks + 1;
}
