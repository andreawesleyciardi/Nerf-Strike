#ifndef TARGET_LIST_SCREEN_H
#define TARGET_LIST_SCREEN_H

#include "../DisplayManager.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

class TargetListScreen : public Screen {
public:
  void render(DisplayManager& display) override {
    display.clear();
    display.showLine(0, "🎯 Target List");
    display.showLine(1, "Paired targets: 3");
    display.showLine(3, "[Back]  [Next]");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right, ScreenManager& screenManager) override {
    if (left.wasPressed()) screenManager.replace(ScreenType::Pairing);
    if (right.wasPressed() || encoder.wasPressed()) screenManager.replace(ScreenType::GameModeList);
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::VerticalScroll;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "Next", ""};
  }

  ScreenType getType() const override {
    return ScreenType::TargetList;
  }
};

#endif