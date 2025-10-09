#ifndef HELP_SCREEN_H
#define HELP_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

// - Help screen:
// Shows available commands or tips.
// Button left: not needed.
// Button right label "Home": navigates to the "Home screen".

class HelpScreen : public Screen {
public:
  HelpScreen(LcdDisplay& display)
   : display(display) {}

  void render() override {
    display.clear();
    display.showLine(0, "Help");
    display.showLine(1, "Commands: list, test, clear");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"", "", "Home"};
  }

  ScreenType getType() const override {
    return ScreenType::Help;
  }

  String getHash() const override {
    return "Help";
  }

private:
  LcdDisplay& display;
};

#endif