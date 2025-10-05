#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class PlayingScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "🔥 Game in Progress");
    display.showLine(1, "Score: 0");
    display.showLine(3, "[Quit]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      screenManager.push(ScreenType::Confirmation);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Quit", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::Playing;
  }
};

#endif