#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"
#include "../GameSessionState.h"

extern GameSessionState session;
extern GameSessionState defaultSession;

// - Home screen:
// I want to show a message (for now just "Welcome").
// Button left label "Settings": navigates to the "Settings screen".
// Button encoder: starts immediately the game with default session settings.
// Button right label "Start": navigates to the "Pairing screen".

class HomeScreen : public Screen {
public:
  HomeScreen(LcdDisplay& display, PairingRegistry& registry)
   : display(display), registry(registry) {}

  void render() override {
    display.clear();
    display.showLine(0, "Home screen");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Settings);
    }
    if (encoder.wasPressed()) {
      session = defaultSession;
      request = ScreenRequest::to(ScreenType::Playing);
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

private:
  LcdDisplay& display;
  PairingRegistry& registry;
};

#endif