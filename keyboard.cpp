#include "keyboard.h"

#include <PS2Keyboard.h>

PS2Keyboard kbd = PS2Keyboard();

void setupKbd() {
  kbd.begin(KBD_DAT, KBD_CLK);
}
