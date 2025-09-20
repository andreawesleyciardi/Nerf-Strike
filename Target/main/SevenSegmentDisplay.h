#ifndef SEVENSEGMENTDISPLAY_H
#define SEVENSEGMENTDISPLAY_H

#include <Arduino.h>
#include <ShiftRegister74HC595.h>

class SevenSegmentDisplay {
  public:
    SevenSegmentDisplay(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
    void setup();
    void showScore(uint8_t score);

  private:
    ShiftRegister74HC595<3> display;
    void showDigit(uint8_t digit, uint8_t position);
};

#endif
