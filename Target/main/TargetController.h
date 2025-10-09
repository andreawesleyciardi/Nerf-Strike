#ifndef TARGET_CONTROLLER_H
#define TARGET_CONTROLLER_H

#include <Arduino.h>
#include "RGBLed.h"

class TargetController {
public:
  TargetController(RGBLed& entityRgbLed);

  void setup(String defaultColor = "White");

  void setEntityColor(String colorName);
  void setEntityBrightness(uint8_t brightness);
  void turnOffEntityLed();

  void blinkEntityColor(String colorName = "White", uint8_t delayTime = 200, uint8_t times = 3);
  void pulseEntityColor(String colorName = "White", uint8_t steps = 20, uint16_t delayMs = 20);

  void updateVisualState(bool isAssigned, bool isActive);

private:
  RGBLed& entityLed;
  String lastColor = "White";
};

#endif