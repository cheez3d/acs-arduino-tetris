#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>

enum GameAct {
  ACT_TET_SHL, // shift tetromino to the left
  ACT_TET_SHR, // shift tetromino to the right
  ACT_TET_ROT, // rotate tetromino clock-wise
  ACT_TET_LWR, // lower tetromino
  ACT_TET_DRP, // hard drop tetromino

  ACT_PAUSE, // pause game

  ACT_NONE,
};

extern uint8_t const GAME_FREQ;

extern bool isGameSetup;
extern bool isGamePaused;
extern bool isGameOver;

extern bool volatile isGameTick;

extern uint16_t rowsClearedCnt;
extern uint8_t lwrPeriod;

#endif // TETRIS_H
