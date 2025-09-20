#include "Piezo.h"

Piezo::Piezo(uint8_t pin, uint16_t threshold, uint16_t debounceMs) {
  sensorPin = pin;
  hitThreshold = threshold;
  debounceTime = debounceMs;
  lastHitTime = 0;
}

void Piezo::setup() {
  pinMode(sensorPin, INPUT);
}

bool Piezo::isHit() {
  int value = analogRead(sensorPin);
  unsigned long currentTime = millis();

  if (value > hitThreshold && (currentTime - lastHitTime) > debounceTime) {
    lastHitTime = currentTime;
    return true;
  }

  return false;
}
