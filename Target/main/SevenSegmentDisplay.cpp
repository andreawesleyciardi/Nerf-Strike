#include "SevenSegmentDisplay.h"

uint8_t numberByte[] = {
  B11000000, //0
  B11111001, //1 
  B10100100, //2
  B10110000, //3 
  B10011001, //4
  B10010010, //5
  B10000011, //6
  B11111000, //7
  B10000000, //8
  B10011000 //9
};

uint8_t* SevenSegmentDisplay::integerToByte(uint8_t value) {
  // digits starts from right to left. Example on number "7953"
  uint8_t digit1,  // 3
    digit2,    // 5
    digit3,    // 9
    digit4;    // 7
  //Split number to digits:
  digit1 = value % 10;
  digit2 = (value / 10) % 10;
  digit3 = (value / 100) % 10;
  digit4 = (value / 1000) % 10;
  switch (numDigits) {
    case 2:
      {
        static uint8_t numberToPrint[2];
        numberToPrint[0] = numberByte[digit2];
        numberToPrint[1] = numberByte[digit1];
        return numberToPrint;
        break;
      }
    case 3:
      {
        static uint8_t numberToPrint[3];
        numberToPrint[0] = numberByte[digit3];
        numberToPrint[1] = numberByte[digit2];
        numberToPrint[2] = numberByte[digit1];
        return numberToPrint;
        break;
      }
    case 4:
      {
        static uint8_t numberToPrint[4];
        numberToPrint[0] = numberByte[digit4];
        numberToPrint[1] = numberByte[digit3];
        numberToPrint[2] = numberByte[digit2];
        numberToPrint[3] = numberByte[digit1];
        return numberToPrint;
        break;
      }
    default:
      {
        static uint8_t numberToPrint[1];
        numberToPrint[0] = numberByte[digit1];
        return numberToPrint;
        break;
      }
  }
}

SevenSegmentDisplay::SevenSegmentDisplay(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
  : display(dataPin, clockPin, latchPin) {}

void SevenSegmentDisplay::setup(uint8_t nDigits) {
  numDigits = nDigits;
  clear(); // Clear display
}

void SevenSegmentDisplay::showScore(uint8_t score) {
  clear(); // Clear before update

  uint8_t* scoreBytes = integerToByte(score);
  display.setAll(scoreBytes);
}

void SevenSegmentDisplay::updateScore(uint8_t score, bool withAnimation) {
  Serial.println("UPDATESCORE");
  if (withAnimation == true) {
    // Flash score digits before stabilizing
    for (int i = 0; i < 3; i++) {
      showScore(score);
      delay(200);
      clear();
      delay(200);
    }
  }

  showScore(score); // Final stable display
}

void SevenSegmentDisplay::clear() {
  display.setAllHigh();
}