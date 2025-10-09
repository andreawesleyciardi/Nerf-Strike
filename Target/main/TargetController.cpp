#include "TargetController.h"

TargetController::TargetController(RGBLed& entityRgbLed)
  : entityLed(entityRgbLed) {}

void TargetController::setup(String defaultColor) {
  lastColor = defaultColor;
  entityLed.setup(false);
  entityLed.setBrightness(255);
  entityLed.off();
}

void TargetController::setEntityColor(String colorName) {
  lastColor = colorName;
  entityLed.on(colorName);
}

void TargetController::setEntityBrightness(uint8_t brightness) {
  entityLed.setBrightness(brightness);
}

void TargetController::turnOffEntityLed() {
  entityLed.off();
}

void TargetController::blinkEntityColor(String colorName, uint8_t delayTime, uint8_t times) {
  entityLed.blink(colorName, delayTime, times);
}

void TargetController::pulseEntityColor(String colorName, uint8_t steps, uint16_t delayMs) {
  entityLed.pulse(colorName, steps, delayMs);
}

void TargetController::updateVisualState(bool isAssigned, bool isActive) {
  if (isAssigned) {
    if (isActive) {
      entityLed.setBrightness(255);
      entityLed.on(lastColor);
    } else {
      entityLed.setBrightness(80);
      entityLed.pulse(lastColor, 20, 20);
    }
  } else {
    entityLed.setBrightness(50);
    entityLed.on("White");
  }
}