#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "ScreenTypes.h"
#include <Arduino.h>

class ScreenManager {
public:
  void setup();
  void push(ScreenID screen);
  void pop();
  void replace(ScreenID screen);

  ScreenID current();
  EncoderMode getEncoderMode();
  ButtonLabels getButtonLabels();

private:
  static const uint8_t maxStackDepth = 5;
  ScreenID stack[maxStackDepth];
  uint8_t stackSize = 0;

  EncoderMode encoderMode = EncoderMode::None;
  ButtonLabels labels = {"", "", ""};

  void updateContext(ScreenID screen);
};

#endif