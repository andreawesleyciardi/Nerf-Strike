#ifndef ERROR_SCREEN_H
#define ERROR_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class ErrorScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "⚠️ Error Occurred");
    display.showLine(1, "Check connections...");
    display.showLine(3, "[Retry]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      screenManager.replace(ScreenType::Home);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Retry", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Error;
  }
};

#endif