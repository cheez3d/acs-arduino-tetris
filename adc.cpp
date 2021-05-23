#include "adc.h"

#include "lcd.h"
#include "tetris.h"

#include <movingAvg.h>

#include <Arduino.h>

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t const ADC_SRCS[ADC_SRC_CNT][ADC_DAT_CNT] = {
  [ADC_BRI] = { (0b0000 << MUX0) /* ADC0 */, 28, 216, 0, 255, 12 },
  [ADC_BUZ] = { (0b0001 << MUX0) /* ADC1 */, 28, 216, 0, 16,  12 },
  [ADC_JSX] = { (0b0011 << MUX0) /* ADC3 */, 20, 216, 0, 255, 1 },
  [ADC_JSY] = { (0b0010 << MUX0) /* ADC2 */, 20, 216, 0, 255, 1 },
};

movingAvg adcSrcs[ADC_SRC_CNT] = {
  movingAvg(ADC_SRCS[ADC_BRI][ADC_AVG]),
  movingAvg(ADC_SRCS[ADC_BUZ][ADC_AVG]),
  movingAvg(ADC_SRCS[ADC_JSX][ADC_AVG]),
  movingAvg(ADC_SRCS[ADC_JSY][ADC_AVG]),
};

static uint8_t src = 0;

void setupADC() {
  src = 0;

  ADCSRA = _BV(ADEN)         // ADC enable
         | _BV(ADSC)         // start conversion
         | _BV(ADATE)        // auto trigger enable
         | _BV(ADIE)         // interrupt enable
         | (0b111 << ADPS0); // prescaler 128

  ADCSRB = (0b101 << ADTS0); // auto trigger on Timer1 compare match B

  ADMUX = 0;

  for (int i = 0; i < ADC_SRC_CNT; ++i) {
    adcSrcs[i].begin();
  }
}

void setADCSrc(uint8_t src) {
  ADMUX = (1 << REFS0)           // AVCC voltage reference with capacitor at AREF
        | (1 << ADLAR)           // left adjust reading
        | ADC_SRCS[src][ADC_CHN]; // analog channel selection bits
}

ISR(ADC_vect) {
  uint8_t val = ADCH;

  val = constrain(val, ADC_SRCS[src][ADC_ILO], ADC_SRCS[src][ADC_IHI]);

  val = map(
    val,
    ADC_SRCS[src][ADC_ILO], ADC_SRCS[src][ADC_IHI],
    ADC_SRCS[src][ADC_OLO], ADC_SRCS[src][ADC_OHI]
  );

  if (src == ADC_BRI && (!isGameSetup || isGamePaused || isGameOver)) {
    val = 0;
  }

  adcSrcs[src].reading(val);

  ++src;

  if (src == ADC_SRC_CNT) {
    src = 0;
  }

  setADCSrc(src);

  TIFR1 |= (1 << OCF1B); // clear output compare B match flag
}
