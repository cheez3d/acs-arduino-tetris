#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <PS2Keyboard.h>

enum KbdPin : uint8_t {
  KBD_CLK = 3,
  KBD_DAT = 4,
};

extern PS2Keyboard kbd;

void setupKbd();

#endif // KEYBOARD_H
