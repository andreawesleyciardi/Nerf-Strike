#ifndef GAME_MODE_OPTIONS_SCREEN_H
#define GAME_MODE_OPTIONS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class GameModeOptionsScreen : public Screen {
public:
  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "⚙️ Game Options");
    display.showLine(1, "Adjust settings...");
    display.showLine(3, "[Back]  [Details]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) screenManager.pop();
    if (right.wasPressed() || encoder.wasPressed()) screenManager.push(ScreenType::GameModeDetails);
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::AdjustField;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "Details", ""};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeOptions;
  }
};

#endif