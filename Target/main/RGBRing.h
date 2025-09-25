#ifndef RGBRING_H
#define RGBRING_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Color.h>

class RGBRing {
  public:
    RGBRing(uint8_t pin, uint16_t numPixels);
    void setup();
    void off();
    void fill(String colorName);
    void blink(String colorName, uint8_t times = 1, uint16_t delayMs = 200);
    void pulse(String colorName, uint8_t steps = 20, uint16_t delayMs = 20);
    void chase(String colorName, uint8_t speed = 50, bool holdLast = false);

  private:
    Adafruit_NeoPixel ring;
    uint16_t pixelCount;
};

#endif
