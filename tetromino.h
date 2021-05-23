#ifndef TETROMINO_H
#define TETROMINO_H

#include <avr/pgmspace.h>

#include <stdint.h>

enum Tet {
  TET_I,
  TET_J,
  TET_L,
  TET_O,
  TET_S,
  TET_T,
  TET_Z,

  TET_CNT,
};

enum TetRot {
  TET_ROT0,
  TET_ROT1,
  TET_ROT2,
  TET_ROT3,

  TET_ROT_CNT,
};

enum TetCol {
  COL_WALL, // collision with a wall
  COL_STACK, // collision with the stack

  COL_NONE,
};

struct TetKick {
  static constexpr uint8_t CNT = 4;

  int8_t dx;
  int8_t dy;
};

struct TetDat {
  uint8_t h; // height
  uint8_t w; // width

  uint8_t const *const *grd;
  TetKick const *const *kicks;
};

extern uint8_t const BEGY; // vertical spawn location for tetrominoes
extern uint8_t const BEGX; // horizontal spawn location for tetrominoes

extern uint8_t const TET_BLK;
extern uint16_t const TET_CL0;
extern uint16_t const TET_CL1;

extern Tet tet;
extern TetRot tetRot;
extern TetDat tetDat;
extern uint8_t tetGrd[4 * 4];
extern TetKick tetKicks[TetKick::CNT];
extern uint8_t tety;
extern uint8_t tetx;
extern uint8_t shdy;
extern uint8_t shdx;

constexpr int8_t toGrd(int8_t n) {
  return n >> 2;
}

constexpr int8_t toLCD(int8_t n) {
  return n << 2;
}

void setupTet();

void setTet(Tet tet);

void setTetRot(TetRot tetRot);
void tetRotCw();
void tetRotCcw();

TetCol tetCol(int8_t dy = 0, int8_t dx = 0);

void tetDraw(uint8_t y, uint8_t x, uint16_t color);

void tetShd();

#endif // TETROMINO_H
