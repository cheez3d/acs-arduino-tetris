#include "tetromino.h"

#include "grid.h"
#include "lcd.h"
#include "tetris.h"

#include <avr/pgmspace.h>

#include <stdint.h>
#include <stdlib.h>

uint8_t const TET_BLK = 4;
uint16_t const TET_CL0 = 0x0000;
uint16_t const TET_CL1 = 0xFFFF;

static constexpr uint8_t TET_IR0[] PROGMEM = {
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,
  0, 0, 0, 0,
};

static constexpr uint8_t TET_IR1[] PROGMEM = {
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
};

static constexpr uint8_t TET_IR2[] PROGMEM = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0,
};

static constexpr uint8_t TET_IR3[] PROGMEM = {
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
  0, 1, 0, 0,
};

static constexpr uint8_t const *TET_IGRD[TET_ROT_CNT] PROGMEM = { TET_IR0, TET_IR1, TET_IR2, TET_IR3 };

static constexpr TetKick TET_I_KICK0[TetKick::CNT] PROGMEM = {
  TetKick { .dx =  1, .dy =  0 },
  TetKick { .dx = -2, .dy =  0 },
  TetKick { .dx =  1, .dy = -2 },
  TetKick { .dx = -2, .dy =  1 },
};

static constexpr TetKick TET_I_KICK1[TetKick::CNT] PROGMEM = {
  TetKick { .dx = -2, .dy =  0 },
  TetKick { .dx =  1, .dy =  0 },
  TetKick { .dx = -2, .dy = -1 },
  TetKick { .dx =  1, .dy =  2 },
};

static constexpr TetKick TET_I_KICK2[TetKick::CNT] PROGMEM = {
  TetKick { .dx = -1, .dy =  0 },
  TetKick { .dx =  2, .dy =  0 },
  TetKick { .dx = -1, .dy =  2 },
  TetKick { .dx =  2, .dy = -1 },
};

static constexpr TetKick TET_I_KICK3[TetKick::CNT] PROGMEM = {
  TetKick { .dx =  2, .dy =  0 },
  TetKick { .dx = -1, .dy =  0 },
  TetKick { .dx =  2, .dy =  1 },
  TetKick { .dx = -1, .dy = -2 },
};

static constexpr TetKick const *TET_I_KICKS[TET_ROT_CNT] PROGMEM = {
  [TET_ROT0] = TET_I_KICK0,
  [TET_ROT1] = TET_I_KICK1,
  [TET_ROT2] = TET_I_KICK2,
  [TET_ROT3] = TET_I_KICK3,
};

static constexpr uint8_t TET_JR0[] PROGMEM = {
  1, 0, 0,
  1, 1, 1,
  0, 0, 0,
};

static constexpr uint8_t TET_JR1[] PROGMEM = {
  0, 1, 1,
  0, 1, 0,
  0, 1, 0,
};

static constexpr uint8_t TET_JR2[] PROGMEM = {
  0, 0, 0,
  1, 1, 1,
  0, 0, 1,
};

static constexpr uint8_t TET_JR3[] PROGMEM = {
  0, 1, 0,
  0, 1, 0,
  1, 1, 0,
};

static constexpr uint8_t const *TET_JGRD[TET_ROT_CNT] PROGMEM = { TET_JR0, TET_JR1, TET_JR2, TET_JR3 };

static constexpr uint8_t TET_LR0[] PROGMEM = {
  0, 0, 1,
  1, 1, 1,
  0, 0, 0,
};

static constexpr uint8_t TET_LR1[] PROGMEM = {
  0, 1, 0,
  0, 1, 0,
  0, 1, 1,
};

static constexpr uint8_t TET_LR2[] PROGMEM = {
  0, 0, 0,
  1, 1, 1,
  1, 0, 0,
};

static constexpr uint8_t TET_LR3[] PROGMEM = {
  1, 1, 0,
  0, 1, 0,
  0, 1, 0,
};

static constexpr uint8_t const *TET_LGRD[TET_ROT_CNT] PROGMEM = { TET_LR0, TET_LR1, TET_LR2, TET_LR3 };

static constexpr uint8_t TET_OR0[] PROGMEM = {
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
};

static constexpr uint8_t TET_OR1[] PROGMEM = {
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
};

static constexpr uint8_t TET_OR2[] PROGMEM = {
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
};

static constexpr uint8_t TET_OR3[] PROGMEM = {
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0,
};

static constexpr uint8_t const *TET_OGRD[TET_ROT_CNT] PROGMEM = { TET_OR0, TET_OR1, TET_OR2, TET_OR3 };

static constexpr uint8_t TET_SR0[] PROGMEM = {
  0, 1, 1,
  1, 1, 0,
  0, 0, 0,
};

static constexpr uint8_t TET_SR1[] PROGMEM = {
  0, 1, 0,
  0, 1, 1,
  0, 0, 1,
};

static constexpr uint8_t TET_SR2[] PROGMEM = {
  0, 0, 0,
  0, 1, 1,
  1, 1, 0,
};

static constexpr uint8_t TET_SR3[] PROGMEM = {
  1, 0, 0,
  1, 1, 0,
  0, 1, 0,
};

static constexpr uint8_t const *TET_SGRD[TET_ROT_CNT] PROGMEM = { TET_SR0, TET_SR1, TET_SR2, TET_SR3 };

static constexpr uint8_t TET_TR0[] PROGMEM = {
  0, 1, 0,
  1, 1, 1,
  0, 0, 0,
};

static constexpr uint8_t TET_TR1[] PROGMEM = {
  0, 1, 0,
  0, 1, 1,
  0, 1, 0,
};

static constexpr uint8_t TET_TR2[] PROGMEM = {
  0, 0, 0,
  1, 1, 1,
  0, 1, 0,
};

static constexpr uint8_t TET_TR3[] PROGMEM = {
  0, 1, 0,
  1, 1, 0,
  0, 1, 0,
};

static constexpr uint8_t const *TET_TGRD[TET_ROT_CNT] PROGMEM = { TET_TR0, TET_TR1, TET_TR2, TET_TR3 };

static constexpr uint8_t TET_ZR0[] PROGMEM = {
  1, 1, 0,
  0, 1, 1,
  0, 0, 0,
};

static constexpr uint8_t TET_ZR1[] PROGMEM = {
  0, 0, 1,
  0, 1, 1,
  0, 1, 0,
};

static constexpr uint8_t TET_ZR2[] PROGMEM = {
  0, 0, 0,
  1, 1, 0,
  0, 1, 1,
};

static constexpr uint8_t TET_ZR3[] PROGMEM = {
  0, 1, 0,
  1, 1, 0,
  1, 0, 0,
};

static constexpr uint8_t const *TET_ZGRD[TET_ROT_CNT] PROGMEM = { TET_ZR0, TET_ZR1, TET_ZR2, TET_ZR3 };

static constexpr TetKick TET_JLOSTZ_KICK0[TetKick::CNT] PROGMEM = {
  TetKick { .dx = -1, .dy =  0 },
  TetKick { .dx = -1, .dy = -1 },
  TetKick { .dx =  0, .dy =  2 },
  TetKick { .dx = -1, .dy =  2 },
};

static constexpr TetKick TET_JLOSTZ_KICK1[TetKick::CNT] PROGMEM = {
  TetKick { .dx = -1, .dy =  0 },
  TetKick { .dx = -1, .dy =  1 },
  TetKick { .dx =  0, .dy = -2 },
  TetKick { .dx = -1, .dy = -2 },
};

static constexpr TetKick TET_JLOSTZ_KICK2[TetKick::CNT] PROGMEM = {
  TetKick { .dx =  1, .dy =  0 },
  TetKick { .dx =  1, .dy = -1 },
  TetKick { .dx =  0, .dy =  2 },
  TetKick { .dx =  1, .dy =  2 },
};

static constexpr TetKick TET_JLOSTZ_KICK3[TetKick::CNT] PROGMEM = {
  TetKick { .dx =  1, .dy =  0 },
  TetKick { .dx =  1, .dy =  1 },
  TetKick { .dx =  0, .dy = -2 },
  TetKick { .dx =  1, .dy = -2 },
};

static constexpr TetKick const *TET_JLOSTZ_KICKS[TET_ROT_CNT] PROGMEM = {
  [TET_ROT0] = TET_JLOSTZ_KICK0,
  [TET_ROT1] = TET_JLOSTZ_KICK1,
  [TET_ROT2] = TET_JLOSTZ_KICK2,
  [TET_ROT3] = TET_JLOSTZ_KICK3,
};

static constexpr TetDat TETS[TET_CNT] PROGMEM = {
  [TET_I] = TetDat { .h = 4, .w = 4, .grd = TET_IGRD, .kicks = TET_I_KICKS },
  [TET_J] = TetDat { .h = 3, .w = 3, .grd = TET_JGRD, .kicks = TET_JLOSTZ_KICKS },
  [TET_L] = TetDat { .h = 3, .w = 3, .grd = TET_LGRD, .kicks = TET_JLOSTZ_KICKS },
  [TET_O] = TetDat { .h = 3, .w = 4, .grd = TET_OGRD, .kicks = TET_JLOSTZ_KICKS },
  [TET_S] = TetDat { .h = 3, .w = 3, .grd = TET_SGRD, .kicks = TET_JLOSTZ_KICKS },
  [TET_T] = TetDat { .h = 3, .w = 3, .grd = TET_TGRD, .kicks = TET_JLOSTZ_KICKS },
  [TET_Z] = TetDat { .h = 3, .w = 3, .grd = TET_ZGRD, .kicks = TET_JLOSTZ_KICKS },
};

uint8_t const BEGY = 0;
uint8_t const BEGX = 4;

Tet tet = TET_CNT;
TetRot tetRot = TET_ROT_CNT;
TetDat tetDat;
uint8_t tetGrd[4 * 4];
TetKick tetKicks[TetKick::CNT];
uint8_t tety;
uint8_t tetx;
uint8_t shdy;
uint8_t shdx;

void setupTet() {
  setTet(static_cast<Tet>(rand() % TET_CNT));

  tety = BEGY;
  tetx = BEGX;

  shdy = tety;
  shdx = tetx;

  TetCol col;

  for (int i = 0; (col = tetCol()) != COL_NONE && i < GRDB; ++i) {
    --tety;
  }

  if (col != COL_NONE) {
    isGameOver = true;
  }
}

void setTet(Tet tet) {
  ::tet = tet;

  memcpy_P(&tetDat, (PGM_P)&TETS[tet], sizeof(tetDat));

  setTetRot(TET_ROT0);
}

void setTetRot(TetRot tetRot) {
  ::tetRot = tetRot;

  memcpy_P(tetGrd, (PGM_P)pgm_read_ptr(&tetDat.grd[tetRot]), tetDat.h * tetDat.w);
  memcpy_P(tetKicks, (PGM_P)pgm_read_ptr(&tetDat.kicks[tetRot]), sizeof(tetKicks));
}

void tetRotCw() {
  setTetRot(static_cast<TetRot>((tetRot + 1) % TET_ROT_CNT));
}

void tetRotCcw() {
  setTetRot(static_cast<TetRot>((((tetRot - 1) % TET_ROT_CNT) + TET_ROT_CNT) % TET_ROT_CNT));
}

TetCol tetCol(int8_t dy, int8_t dx) {
  for (uint8_t ty = 0; ty < tetDat.h; ++ty) {
    for (uint8_t tx = 0; tx < tetDat.w; ++tx) {
      if (tetGrd[ty * tetDat.w + tx]) {
        int8_t y = tety + ty + dy;
        int8_t x = tetx + tx + dx;

        if (grd[grdy(y)][grdx(x)]) {
          if (dy == 0 && dx != 0) {
            return COL_WALL;
          }
          else {
            return COL_STACK;
          }
        }
      }
    }
  }

  return COL_NONE;
}

void tetDraw(uint8_t y, uint8_t x, uint16_t color) {
  for (uint8_t ty = 0; ty < tetDat.h; ++ty) {
    for (uint8_t tx = 0; tx < tetDat.w; ++tx) {
      if (tetGrd[ty * tetDat.w + tx]) {
        int8_t lx = toLCD(x + tx);
        int8_t ly = toLCD(y + ty);

        lcd.drawRect(lx, ly, TET_BLK, TET_BLK, color);
      }
    }
  }
}

void tetShd() {
  shdy = tety;
  shdx = tetx;

  while (tetCol(shdy - tety + 1, 0) == COL_NONE) {
    ++shdy;
  }
}
