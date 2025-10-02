#ifndef GAME_MODE_LIST_SCREEN_H
#define GAME_MODE_LIST_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class GameModeListScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "🎮 Select Game Mode");
    display.showLine(3, "[Back]  [Select]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) screenManager.pop();
    if (right.wasPressed() || encoder.wasPressed()) screenManager.push(ScreenType::GameModeOptions);
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::SelectItem;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "Select", ""};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeList;
  }
};

#endif