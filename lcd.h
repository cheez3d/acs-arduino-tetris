#ifndef LCD_H
#define LCD_H

#include <Adafruit_PCD8544.h>

enum LCDPin {
  LCD_BKL = 6,
  LCD_CLK = 13,
  LCD_DIN = 11,
  LCD_DC  = 8,
  LCD_RST = 9,
  LCD_CE  = 10,
};

extern Adafruit_PCD8544 lcd;

void setupLCD();

void lcdSplash();
void lcdOver();

#endif // LCD_H
