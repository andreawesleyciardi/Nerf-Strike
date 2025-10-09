#ifndef CONFIRMATION_SCREEN_H
#define CONFIRMATION_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../HubStateManager.h"
#include "Screen.h"
#include "../GameSessionState.h"

extern GameSessionState session;

// - Confirmation screen:
// A message with a question is shown.
// Button left label "Cancel": negates whatever question is shown to the user. Navigates to screen to define based on the question.
// Button right label "Confirm": validates whatever question is shown to the user. Navigates to screen to define based on the question.

class ConfirmationScreen : public Screen {
public:
  ConfirmationScreen(LcdDisplay& display, HubStateManager& hubState)
    : display(display), hubState(hubState) {}

  void render() override {
    display.clear();
    display.showLine(0, "Confirm Exit?");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      // negates whatever question is shown to the user. Navigates to screen to define based on the question.
    }
    if (right.wasPressed()) {
      // validates whatever question is shown to the user. Navigates to screen to define based on the question.
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Cancel", "", "Confirm"};
  }

  ScreenType getType() const override {
    return ScreenType::Confirmation;
  }

  String getHash() const override {
    String hash = "Confirmation";
    hash += "-" + String(hubState.getCurrentMenuIndex());
    return hash;
  }

private:
  LcdDisplay& display;
  HubStateManager& hubState;
};

#endif