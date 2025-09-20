#include "SevenSegmentDisplay.h"

// Segment encoding for common cathode 7-segment display
const byte digitSegments[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

SevenSegmentDisplay::SevenSegmentDisplay(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
  : display(dataPin, clockPin, latchPin) {}

void SevenSegmentDisplay::setup() {
  display.setAll(0); // Clear display
}

void SevenSegmentDisplay::showDigit(uint8_t digit, uint8_t position) {
  if (digit > 9 || position > 2) return;
  display.set(position, digitSegments[digit]);
}

void SevenSegmentDisplay::showScore(uint8_t score) {
  display.setAll(0); // Clear before update

  uint8_t hundreds = score / 100;
  uint8_t tens = (score / 10) % 10;
  uint8_t ones = score % 10;

  if (hundreds > 0) showDigit(hundreds, 0);
  if (hundreds > 0 || tens > 0) showDigit(tens, 1);
  showDigit(ones, 2);
}
