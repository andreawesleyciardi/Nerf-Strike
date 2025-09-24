#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  private:
    uint8_t pin;

  public:
    Buzzer(uint8_t buzzerPin);
    void setup();
    void beep(unsigned int durationMs);
    void tone(unsigned int frequency, unsigned int durationMs);
    void off();
};

#endif
