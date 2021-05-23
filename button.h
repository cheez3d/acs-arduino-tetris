#ifndef BUTTON_H
#define BUTTON_H

enum BtnPin {
  BTN_PIN = 7,
};

extern bool volatile isBtnLow;
extern bool volatile isBtn;

void setupBtn();

bool getBtn();

#endif // BUTTON_H
