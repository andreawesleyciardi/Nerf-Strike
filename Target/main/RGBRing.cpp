#include <Color.h>
#include <ColorPalette.h>

#include "RGBRing.h"

RGBRing::RGBRing(uint8_t pin, uint16_t numPixels)
  : ring(numPixels, pin, NEO_GRB + NEO_KHZ800), pixelCount(numPixels) {}

void RGBRing::setup() {
  ring.begin();
  ring.show(); // Initialize all pixels to 'off'
}

void RGBRing::off() {
  ring.clear();
  ring.show();
}

void RGBRing::fill(String colorName) {
  Color c = ColorPalette::getByName(colorName);
  for (uint16_t i = 0; i < pixelCount; i++) {
    ring.setPixelColor(i, ring.Color(c.red, c.green, c.blue));
  }
  ring.show();
}

void RGBRing::blink(String colorName, uint8_t times, uint16_t delayMs) {
  for (uint8_t i = 0; i < times; i++) {
    fill(colorName);
    delay(delayMs);
    off();
    delay(delayMs);
  }
}

void RGBRing::pulse(String colorName, uint8_t steps, uint16_t delayMs) {
  Color c = ColorPalette::getByName(colorName);
  for (uint8_t i = 0; i <= steps; i++) {
    uint8_t r = (c.red * i) / steps;
    uint8_t g = (c.green * i) / steps;
    uint8_t b = (c.blue * i) / steps;
    for (uint16_t j = 0; j < pixelCount; j++) {
      ring.setPixelColor(j, ring.Color(r, g, b));
    }
    ring.show();
    delay(delayMs);
  }
  for (int i = steps; i >= 0; i--) {
    uint8_t r = (c.red * i) / steps;
    uint8_t g = (c.green * i) / steps;
    uint8_t b = (c.blue * i) / steps;
    for (uint16_t j = 0; j < pixelCount; j++) {
      ring.setPixelColor(j, ring.Color(r, g, b));
    }
    ring.show();
    delay(delayMs);
  }
}

void RGBRing::chase(String colorName, uint8_t speed, bool holdLast) {
  Color c = ColorPalette::getByName(colorName);
  for (uint16_t i = 0; i < pixelCount; i++) {
    ring.clear();
    ring.setPixelColor(i, ring.Color(c.red, c.green, c.blue));
    ring.show();
    delay(speed);
  }

  if (!holdLast) {
    ring.clear();
    ring.show();
  }
}

