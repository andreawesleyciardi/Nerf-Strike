#include "Buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin) {
  pin = buzzerPin;
}

void Buzzer::setup() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void Buzzer::beep(unsigned int durationMs) {
  tone(pin, 100);  // 1kHz tone
  delay(durationMs);
  noTone(pin);
}

void Buzzer::tone(unsigned int frequency, unsigned int durationMs) {
  ::tone(pin, frequency);
  delay(durationMs);
  noTone(pin);
}

void Buzzer::off() {
  noTone(pin);
  digitalWrite(pin, LOW);
}
