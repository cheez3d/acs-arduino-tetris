#ifndef GRID_H
#define GRID_H

#include "lcd.h"
#include "tetromino.h"

#include <stdint.h>

enum GrdDim {
  GRDB = 2, // number of invisible rows at the top
  GRDI = 1, // thickness of invisible border for wall collision detection
  GRDH = toGrd(LCDWIDTH), // grid height
  GRDW = toGrd(LCDHEIGHT), // grid witdh
};

extern uint8_t grd[GRDB + GRDH + GRDI][GRDI + GRDW + GRDI];

constexpr uint8_t grdx(int8_t gx) {
  return GRDI + gx;
}

constexpr uint8_t grdy(int8_t gy) {
  return GRDB + gy;
}

void setupGrd();

void grdAddTet();

void grdDraw(uint16_t color);

#endif // GRID_H
