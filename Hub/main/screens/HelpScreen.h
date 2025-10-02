#ifndef HELP_SCREEN_H
#define HELP_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class HelpScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "❓ Help");
    display.showLine(1, "Commands: list, test, clear");
    display.showLine(3, "[Back]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      screenManager.pop();
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Help;
  }
};

#endif