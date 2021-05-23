#ifndef ADC_H
#define ADC_H

#include <movingAvg.h>

#include <stdint.h>

enum ADCSrc {
  ADC_BRI, // brightness control knob
  ADC_BUZ, // buzzer volume control knob
  ADC_JSX, // joystick horizontal axis
  ADC_JSY, // joystick vertical axis

  ADC_SRC_CNT,
};

enum ADCDat {
  ADC_CHN, // source channel
  ADC_ILO, // source minimum readout
  ADC_IHI, // source maximum readout
  ADC_OLO, // output minimum value
  ADC_OHI, // output maximum value
  ADC_AVG, // source readout count

  ADC_DAT_CNT,
};

extern uint8_t const ADC_SRCS[ADC_SRC_CNT][ADC_DAT_CNT];

extern movingAvg adcSrcs[ADC_SRC_CNT];

void setupADC();

void setADCSrc(uint8_t src);

#endif // ADC_H
