#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor {
  public:
    Sensor(uint8_t pin, uint16_t threshold = 100, uint16_t debounceMs = 200);
    void setup();
    bool isHit();

  private:
    uint8_t sensorPin;
    uint16_t hitThreshold;
    uint16_t debounceTime;
    unsigned long lastHitTime;
};

#endif
