#include "timer1.h"

#include "adc.h"
#include "button.h"
#include "buzzer.h"
#include "joystick.h"
#include "lcd.h"
#include "tetris.h"

#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t const TIMR_FREQ = GAME_FREQ * ADC_SRC_CNT;

static bool wasBtnLow;
static bool wasJSBtnLow;

static uint8_t counter;

void setupTimer1() {
  wasBtnLow = isBtnLow;
  wasJSBtnLow = isJSBtnLow;

  counter = 0;

  ICR1  = F_CPU / (TIMR_FREQ * 64) - 1;
  OCR1A = ICR1;
  OCR1B = ICR1;

  TCCR1A = 0;

  TCCR1B = (0b11 << WGM12)  // CTC mode with ICR1 as TOP
         | (0b011 << CS10); // prescaler 64

  TCCR1C = 0;

  TCNT1 = 0;

  TIMSK1 = _BV(OCIE1A); // output compare match A interrupt
}

ISR(TIMER1_COMPA_vect) {
  if (!isGameSetup) {
    return;
  }

  if (isBtnLow) {
    if (wasBtnLow) {
      if (digitalRead(BTN_PIN) == LOW) {
        isBtn = true;
      }

      isBtnLow = false;
      wasBtnLow = false;
    }
    else {
      wasBtnLow = true;
    }
  }

  if (isJSBtnLow) {
    if (wasJSBtnLow) {
      if (digitalRead(JS_BTN) == LOW) {
        isJSBtn = true;
      }

      isJSBtnLow = false;
      wasJSBtnLow = false;
    }
    else {
      wasJSBtnLow = true;
    }
  }

  bool isBuzzer = buzTicks > 0;

  analogWrite(LCD_BKL, adcSrcs[ADC_BRI].getAvg());

  if (isBuzzer) {
    analogWrite(BUZ_OUT, adcSrcs[ADC_BUZ].getAvg());
  }

  isGameTick = isGameTick || (counter % ADC_SRC_CNT == 0);

  if (isGameTick) {
    if (isBuzzer && --buzTicks == 0) {
      digitalWrite(BUZ_OUT, LOW);
    }
  }

  ++counter;
}
