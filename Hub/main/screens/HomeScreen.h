#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class HomeScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "🏠 Welcome");
    display.showLine(1, "This is the Home screen");
    display.showLine(3, "[Help]  [Settings]  [Start]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) {
      screenManager.replace(ScreenType::Help);
    }
    if (encoder.wasPressed()) {
      screenManager.replace(ScreenType::Settings);
    }
    if (right.wasPressed()) {
      screenManager.replace(ScreenType::Pairing);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Help", "Start", "Settings"};
  }

  ScreenType getType() const override {
    return ScreenType::Home;
  }
};

#endif