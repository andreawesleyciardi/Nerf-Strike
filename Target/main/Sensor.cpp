#include "Sensor.h"

Sensor::Sensor(uint8_t pin, uint16_t threshold, uint16_t debounceMs) {
  sensorPin = pin;
  hitThreshold = threshold;
  debounceTime = debounceMs;
  lastHitTime = 0;
}

void Sensor::setup() {
  pinMode(sensorPin, INPUT);
}

bool Sensor::isHit() {
  int value = analogRead(sensorPin);
  unsigned long currentTime = millis();

  if (value > hitThreshold && (currentTime - lastHitTime) > debounceTime) {
    lastHitTime = currentTime;
    return true;
  }

  return false;
}
