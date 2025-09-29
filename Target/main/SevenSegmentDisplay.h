#ifndef SEVENSEGMENTDISPLAY_H
#define SEVENSEGMENTDISPLAY_H

#include <Arduino.h>
#include <ShiftRegister74HC595.h>

class SevenSegmentDisplay {
  public:
    SevenSegmentDisplay(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
    void setup(uint8_t nDigits = 3);
    void showScore(uint8_t score);
    void updateScore(uint8_t score, bool withAnimation = true);
    void clear();

  private:
    ShiftRegister74HC595<3> display;
    uint8_t numDigits;
    uint8_t* integerToByte(uint8_t value);
};

#endif
