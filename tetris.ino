#include "tetris.h"

#include "adc.h"
#include "button.h"
#include "buzzer.h"
#include "grid.h"
#include "joystick.h"
#include "keyboard.h"
#include "lcd.h"
#include "tetromino.h"
#include "timer1.h"

#include <avr/wdt.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <PS2Keyboard.h>

uint8_t const GAME_FREQ = 50; // game ticks per second

bool isGameSetup;
bool isGamePaused;
bool isGameOver;

bool volatile isGameTick;
static uint8_t gameTick;

uint16_t rowsClearedCnt;
uint8_t lwrPeriod;
static uint8_t lwrGameTick;

static TetCol col;

static GameAct getGameAct() {
  static GameAct gameActLast = ACT_NONE;
  static uint8_t repeatPeriod;
  static uint8_t repeatGameTick;

  switch (getJSDir()) {
  case JS_N:
    return (gameActLast = ACT_TET_ROT);

  case JS_S: {
    repeatGameTick = (gameTick + 10) % GAME_FREQ;
    repeatPeriod = 5;

    return (gameActLast = ACT_TET_LWR);
  }

  case JS_W: {
    repeatGameTick = (gameTick + 15) % GAME_FREQ;
    repeatPeriod = 5;

    return (gameActLast = ACT_TET_SHL);
  }

  case JS_E: {
    repeatGameTick = (gameTick + 15) % GAME_FREQ;
    repeatPeriod = 5;

    return (gameActLast = ACT_TET_SHR);
  }

  default:
    if (jsDirLast == JS_S || jsDirLast == JS_W || jsDirLast == JS_E) {
      if (gameTick == repeatGameTick) {
        repeatGameTick = GAME_FREQ;

        return gameActLast;
      }
      else if (repeatGameTick == GAME_FREQ && gameTick % repeatPeriod == 0) {
        return gameActLast;
      }
    }
  }

  if (getJSBtn()) {
    return (gameActLast = ACT_TET_DRP);
  }

  if (getBtn()) {
    return (gameActLast = ACT_PAUSE);
  }

  if (kbd.available()) {
    switch (kbd.read()) {
    case PS2_LEFTARROW:
    case 'a':
      return (gameActLast = ACT_TET_SHL);

    case PS2_RIGHTARROW:
    case 'd':
      return (gameActLast = ACT_TET_SHR);

    case PS2_UPARROW:
    case 'w':
      return (gameActLast = ACT_TET_ROT);

    case PS2_DOWNARROW:
    case 's':
      return (gameActLast = ACT_TET_LWR);

    case ' ':
      return (gameActLast = ACT_TET_DRP);

    case 'p':
      return (gameActLast = ACT_PAUSE);
    }
  }

  return ACT_NONE;
}

/* static void reset() {
  cli();

  wdt_enable(WDTO_15MS);

  while (true); // wait for watchdog to perform reset
} */

static void (*reset)() = 0;

void setup() {
  cli();

  // Serial.begin(9600);

  isGameSetup = false;
  isGamePaused = false;
  isGameOver = false;

  isGameTick = false;
  gameTick = 0;

  rowsClearedCnt = 0;
  lwrPeriod = 50;
  lwrGameTick = 0;

  col = COL_NONE;

  unsigned seed = 0;

  for (int i = 0; i < 10; ++i) {
    for (uint8_t pin = A0; pin <= A5; ++pin) {
      seed += analogRead(pin);
    }
  }

  srand(seed);

  setupGrd();
  setupTet();
  setupTimer1();
  setupADC();
  setupBtn();
  setupJS();
  setupKbd();
  setupLCD();
  setupBuz();

  setADCSrc(0);

  sei();

  delay(1000); // wait for ADC values to settle

  isGameSetup = true;

  delay(2000); // show splash screen

  lcd.clearDisplay();
}

void loop() {
  if (!isGameTick) {
    return;
  }

  GameAct gameAct = getGameAct();

  int8_t dy = 0;
  int8_t dx = 0;

  if (isGamePaused) {
    if (gameAct == ACT_PAUSE) {
      isGamePaused = false;

      setBuz(5);
    }

    goto game_loop_end;
  }

  if (isGameOver) {
    lcdOver();

    delay(2000);

    reset();
  }

  tetDraw(tety, tetx, TET_CL0); // clear tetromino from old position
  tetDraw(shdy, shdx, TET_CL0);

  if (gameAct != ACT_NONE && tetCol(1, 0) == COL_STACK) {
    // allow player to keep tetromino from colliding with stack
    // if it would otherwise collide
    // (for as long as the player keeps moving the tetromino)
    lwrGameTick = (gameTick + GAME_FREQ) % GAME_FREQ;
  }
  else if (gameTick == lwrGameTick) {
    dy = 1;

    lwrGameTick = (lwrGameTick + lwrPeriod) % GAME_FREQ;
  }

  switch (gameAct) {
  case ACT_TET_SHL:
    dx = dy == 0 ? -1 : 0;

    break;

  case ACT_TET_SHR:
    dx = dy == 0 ? 1 : 0;

    break;

  case ACT_TET_ROT:
    tetRotCw();

    break;

  case ACT_TET_DRP:
    tety = shdy;

  case ACT_TET_LWR:
    dy = 1;

    break;

  case ACT_PAUSE:
    isGamePaused = true;

    setBuz(5);

    break;
  }

  col = tetCol(dy, dx);

  if (col != COL_NONE) {
    if (gameAct == ACT_TET_ROT) {
      for (int i = 0; i < TetKick::CNT; ++i) {
        dy = tetKicks[i].dy;
        dx = tetKicks[i].dx;

        if (tetCol(dy, dx) == COL_NONE) {
          goto game_loop_tet_move;
        }
      }

      tetRotCcw(); // wall-kick failed, revert
    }

    dy = 0;
    dx = 0;

    if (gameAct == ACT_TET_ROT) {
      goto game_loop_tet_move;
    }

    if (col == COL_STACK) {
      grdAddTet();

      setupTet();

      setBuz(5);
    }
  }

game_loop_tet_move:
  tety += dy;
  tetx += dx;

  tetShd();

  tetDraw(tety, tetx, TET_CL1); // draw tetromino

  if (shdy - tety > tetDat.h) {
    tetDraw(shdy, shdx, TET_CL1);
  }

  lcd.display();

  ++gameTick;

  if (gameTick == GAME_FREQ) {
    gameTick = 0;
  }

game_loop_end:
  isGameTick = false;
}
