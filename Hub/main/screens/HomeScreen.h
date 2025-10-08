#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"

// - Home screen:
// I want to show a message (for now just "Welcome").
// Button left label "Help": navigates to the "Help screen".
// Button encoder: navigates to "Setting screen".
// Button right label "Start": navigates to the "Pairing screen".

class HomeScreen : public Screen {
public:
  HomeScreen() {}

  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "Welcome");
    display.showLine(1, "This is the Home screen");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Help);
    }
    if (encoder.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Settings);
    }
    if (right.wasPressed() || encoder.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Pairing);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Help", "<>", "Start"};
  }

  ScreenType getType() const override {
    return ScreenType::Home;
  }

  String getHash() const {
    return "Home";
  }
};

#endif