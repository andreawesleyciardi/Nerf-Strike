#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class SettingsScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "⚙️ Settings");
    display.showLine(1, "Adjust brightness, sound...");
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
    return ScreenType::Settings;
  }
};

#endif