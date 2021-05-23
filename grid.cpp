#include "grid.h"

#include "buzzer.h"
#include "lcd.h"
#include "tetris.h"
#include "tetromino.h"

#include <Arduino.h>

#include <stdint.h>
#include <string.h>

uint8_t grd[GRDB + GRDH + GRDI][GRDI + GRDW + GRDI];

void setupGrd() {
  memset(grd, 0, sizeof(grd));

  // initialize invisible out-of-screen border
  // for easier wall collision detection

  for (uint8_t gy = 0; gy < GRDH; ++gy) {
    grd[grdy(gy)][grdx(-1)]   = 1;
    grd[grdy(gy)][grdx(GRDW)] = 1;
  }

  for (uint8_t gx = 0; gx < GRDW; ++gx) {
    grd[grdy(GRDH)][grdx(gx)] = 1;
  }
}

void grdAddTet() {
  for (uint8_t ty = 0; ty < tetDat.h; ++ty) {
    for (uint8_t tx = 0; tx < tetDat.w; ++tx) {
      if (tetGrd[ty * tetDat.w + tx]) {
        grd[grdy(tety + ty)][grdx(tetx + tx)] = 1;
      }
    }
  }

  for (uint8_t ty = 0; ty < tetDat.h; ++ty) {
    uint8_t tx;

    for (tx = 0; tx < tetDat.w; ++tx) {
      if (tetGrd[ty * tetDat.w + tx]) {
        break;
      }
    }

    if (tx == tetDat.w) {
      continue;
    }

    uint8_t gy = grdy(tety + ty);

    uint8_t x;

    for (x = 0; x < GRDW; ++x) {
      if (!grd[gy][grdx(x)]) {
        break;
      }
    }

    if (x == GRDW) { // full row
      grdDraw(TET_CL0); // clear old grid from screen

      for (x = 0; x < GRDW; ++x) {
        grd[gy][grdx(x)] = 0;
      }

      grdDraw(TET_CL1); // draw grid with full row removed

      lcd.display();

      setBuz(15);
      delay(250);

      grdDraw(TET_CL0); // clear grid with full row removed

      // shift down everything from above the row
      for (uint8_t gyy = gy; gyy > 0; --gyy) {
        for (x = 0; x < GRDW; ++x) {
          grd[gyy][grdx(x)] = grd[gyy - 1][grdx(x)];
        }
      }

      ++rowsClearedCnt;

      if (rowsClearedCnt % 1 == 0 && lwrPeriod > 5) {
        --lwrPeriod;
      }
    }
  }

  grdDraw(TET_CL1);
}

void grdDraw(uint16_t color) {
  for (uint8_t gy = 0; gy < GRDH; ++gy) {
    for (uint8_t gx = 0; gx < GRDW; ++gx) {
      if (grd[grdy(gy)][grdx(gx)]) {
        lcd.fillRect(toLCD(gx), toLCD(gy), TET_BLK, TET_BLK, color);
      }
    }
  }
}
