#ifndef PAIRING_SCREEN_H
#define PAIRING_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class PairingScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.printAligned("Pairing...", 1, "center");
    display.showLine(3, "[Cancel]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      screenManager.pop();
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Cancel", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Pairing;
  }
};

#endif