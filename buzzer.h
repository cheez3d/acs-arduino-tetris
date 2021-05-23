#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

enum BuzPin {
  BUZ_OUT = 5,
};

extern uint8_t buzTicks;

void setupBuz();

void setBuz(uint8_t ticks);

#endif // BUZZER_H
