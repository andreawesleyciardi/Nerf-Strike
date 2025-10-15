#ifndef GAME_MODE_DETAILS_SCREEN_H
#define GAME_MODE_DETAILS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameModeRegistry.h"
#include "Screen.h"
#include "../GameSessionManager.h"

extern GameSessionManager sessionManager;

// - GameMode Details screen:
// Shows a text with the description of the game mode.
// Button left label "Back": navigates to the "GameModeList screen".
// Button right label "Start": navigates to the "Playing screen".

class GameModeDetailsScreen : public Screen {
public:
  GameModeDetailsScreen(LcdDisplay& display, GameModeRegistry& gameModes)
    : display(display), gameModes(gameModes) {}

  void render() override {
    display.clear();
    display.showLine(0, "Mode Details");
    display.showLine(1, "Description, rules...");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Playing);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "", "Start"};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeDetails;
  }

  String getHash() const override {
    const GameMode& mode = gameModes.getMode(selectedIndex);
    String hash = "GameModeDetails-" + mode.getName();
    for (uint8_t i = 0; i < mode.getSettingCount(); i++) {
      const ModeSetting& setting = mode.getSetting(i);
      hash += "-" + setting.label + ":" + String(setting.value);
    }
    return hash;
  }

private:
  LcdDisplay& display;
  GameModeRegistry& gameModes;
  uint8_t selectedIndex = 0;  // You can update this externally if needed
};

#endif