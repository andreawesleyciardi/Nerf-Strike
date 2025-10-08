#ifndef SETTINGS_SCREEN_H
#define SETTINGS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../HubStateManager.h"
#include "Screen.h"

class SettingsScreen : public Screen {
public:
  SettingsScreen(HubStateManager& hubState)
    : hubState(hubState) {}

  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "⚙️ Settings");
    display.showLine(1, "Adjust brightness, sound...");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed() || right.wasPressed() || encoder.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "<>", ""};
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
  HubStateManager& hubState;
};

#endif