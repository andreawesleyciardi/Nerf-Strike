#ifndef CONFIRMATION_SCREEN_H
#define CONFIRMATION_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class ConfirmationScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "❗ Confirm Exit?");
    display.showLine(3, "[Cancel]  [Exit]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) screenManager.pop();
    if (right.wasPressed() || encoder.wasPressed()) screenManager.replace(ScreenType::Home);
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Cancel", "Exit", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Confirmation;
  }
};

#endif