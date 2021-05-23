#include "joystick.h"

#include "adc.h"

#include <Arduino.h>

JSDir jsDirLast;
bool volatile isJSBtnLow;
bool volatile isJSBtn;

static void onJSBtn() {
  isJSBtnLow = true;
}

void setupJS() {
  pinMode(JS_BTN, INPUT_PULLUP);

  jsDirLast = JS_O;
  isJSBtnLow = false;
  isJSBtn = false;

  attachInterrupt(digitalPinToInterrupt(JS_BTN), onJSBtn, FALLING);
}

JSDir getJSDir() {
  // move factors: affect how much the joystick has to move in a particular
  // direction before the threshold is reached and a direction change is triggered
  static constexpr uint8_t JSX_MVF = 63;
  static constexpr uint8_t JSY_MVF = 63;

  static uint8_t const JSX_TLO = ADC_SRCS[ADC_JSX][ADC_OHI] / (JSX_MVF + 1);
  static uint8_t const JSX_THI = JSX_TLO * JSX_MVF;
  static uint8_t const JSY_TLO = ADC_SRCS[ADC_JSY][ADC_OHI] / (JSY_MVF + 1);
  static uint8_t const JSY_THI = JSY_TLO * JSY_MVF;

  JSDir dir = (adcSrcs[ADC_JSY].getAvg() <= JSY_TLO) ? JS_N
            : (adcSrcs[ADC_JSY].getAvg() >  JSY_THI) ? JS_S
            : (adcSrcs[ADC_JSX].getAvg() <= JSX_TLO) ? JS_E
            : (adcSrcs[ADC_JSX].getAvg() >  JSX_THI) ? JS_W
            :                                          JS_O;

  return (dir != jsDirLast) ? (jsDirLast = dir) : JS_O;
}

bool getJSBtn() {
  bool wasJSBtn = isJSBtn;

  isJSBtn = false;

  return wasJSBtn;
}
