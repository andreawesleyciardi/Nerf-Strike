#ifndef ERROR_SCREEN_H
#define ERROR_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../HubStateManager.h"
#include "Screen.h"

// - Error screen:
// Show error messages or failed actions.
// Button left: to define based on the error.
// Button right: to define based on the error.

class ErrorScreen : public Screen {
public:
  ErrorScreen(HubStateManager& hubState)
    : hubState(hubState) {}

  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "⚠️ Error Occurred");
    display.showLine(1, "Check connections...");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      // to define based on the error.
    }
    if (right.wasPressed()) {
      // to define based on the error.
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Cancel", "", "OK"};
  }

  ScreenType getType() const override {
    return ScreenType::Error;
  }

  String getHash() const override {
    return "Error-Something";
    // return "Error-" + hubState.getLastErrorMessage();
  }

private:
  HubStateManager& hubState;
};

#endif