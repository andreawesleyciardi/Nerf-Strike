#ifndef GAME_MODE_DETAILS_SCREEN_H
#define GAME_MODE_DETAILS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class GameModeDetailsScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "📋 Mode Details");
    display.showLine(1, "Description, rules...");
    display.showLine(3, "[Back]  [Play]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) screenManager.pop();
    if (right.wasPressed() || encoder.wasPressed()) screenManager.push(ScreenType::Playing);
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "Play", ""};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeDetails;
  }
};

#endif