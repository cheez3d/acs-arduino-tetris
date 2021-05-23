#ifndef JOYSTICK_H
#define JOYSTICK_H

enum JSPin {
  JS_BTN = 2,
};

enum JSDir { JS_W, JS_E, JS_N, JS_S, JS_O };

extern JSDir jsDirLast;
extern bool volatile isJSBtnLow;
extern bool volatile isJSBtn;

void setupJS();

JSDir getJSDir();
bool getJSBtn();

#endif // JOYSTICK_H
