#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../HubStateManager.h"
#include "Screen.h"

// - Settings screen:
// I want to show a list of general settings like for example the type of Targets allowed.
// Button left label "Back": navigates to the "Home screen".
// Button right label "Help": navigates to the "Help screen".

class SettingsScreen : public Screen {
public:
  SettingsScreen(LcdDisplay& display, HubStateManager& hubState)
    : display(display), hubState(hubState) {}

  void render() override {
    display.clear();
    display.showLine(0, "Settings");
    display.showLine(1, "Adjust brightness, sound...");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Help);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "", "Help"};
  }

  ScreenType getType() const override {
    return ScreenType::Settings;
  }

  String getHash() const override {
    String hash = "Settings";
    // hash += "-Brightness:" + String(hubState.getBrightness());
    // hash += "-Tier:" + String((int)hubState.getProductTier());
    return hash;
  }

private:
  LcdDisplay& display;
  HubStateManager& hubState;
};

#endif