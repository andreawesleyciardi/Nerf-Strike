#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "ScreenTypes.h"
#include "EncoderMode.h"
#include "ButtonLabels.h"
#include "screens/Screen.h"
#include <Arduino.h>

class ScreenManager {
public:
  void setup();
  void push(ScreenType screen);
  void pop();
  void replace(ScreenType screen);

  ScreenType current();
  Screen* getActive();
  EncoderMode getEncoderMode();
  ButtonLabels getButtonLabels();

private:
  static const uint8_t maxStackDepth = 5;
  ScreenType stack[maxStackDepth];
  uint8_t stackSize = 0;

  Screen* screens[12];  // One for each ScreenType enum value
  EncoderMode encoderMode = EncoderMode::None;
  ButtonLabels labels = {"", "", ""};

  void updateContext(ScreenType screen);
};

#endif