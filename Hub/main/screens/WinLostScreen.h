#ifndef WIN_LOST_SCREEN_H
#define WIN_LOST_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class WinLostScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "🏆 Game Over");
    display.showLine(1, "You won!");
    display.showLine(3, "[Home]");
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
    return {"Home", "", ""};
  }

  ScreenType getType() const override {
    return ScreenType::WinLost;
  }
};

#endif