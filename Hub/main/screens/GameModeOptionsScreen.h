#ifndef GAME_MODE_OPTIONS_SCREEN_H
#define GAME_MODE_OPTIONS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameModeRegistry.h"
#include "Screen.h"

// - GameMode Options screen:
// Some game modes will have extra options.
// Some examples:
// when the game mode is "To number" the user will be able to, starting from a default number, change the value of the number of hits that has to reach for complete the game but also .
// when the game mode is "" 
// Button left label "Back": navigates to the "GameModeList screen".
// Button encoder: pressed first time focuses on a field so that the value can be changed with the scrolling of the encoder, next press the focus is removed and the scrolling of the encoder is used again for scroll vertically the list of fields.
// Button right label "Select": navigates to the "GameMode Details screen with the values of the options in memory".

class GameModeOptionsScreen : public Screen {
public:
  GameModeOptionsScreen(GameModeRegistry& gameModes)
    : gameModes(gameModes) {}

  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "⚙️ Game Options");
    display.showLine(1, "Adjust settings...");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
    }
    if (encoder.wasPressed()) {
      // pressed first time focuses on a field so that the value can be changed with the scrolling of the encoder, next press the focus is removed and the scrolling of the encoder is used again for scroll vertically the list of fields.
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeDetails);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::AdjustField;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "<>", "Select"};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeOptions;
  }

  String getHash() const override {
    const GameMode& mode = gameModes.getMode(selectedIndex);
    String hash = "GameModeOptions-" + mode.getName();
    for (uint8_t i = 0; i < mode.getSettingCount(); i++) {
      const ModeSetting& setting = mode.getSetting(i);
      hash += "-" + setting.label + ":" + String(setting.value);
    }
    return hash;
  }

private:
  GameModeRegistry& gameModes;
  uint8_t selectedIndex = 0;  // You can update this externally if needed
};

#endif