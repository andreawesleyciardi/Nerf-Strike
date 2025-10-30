#ifndef GAME_MODE_OPTIONS_SCREEN_H
#define GAME_MODE_OPTIONS_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameMode.h"
#include "../GameModeRegistry.h"
#include "../GameSessionManager.h"
#include "Screen.h"

extern GameSessionManager sessionManager;
extern ScreenRequest request;

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
  GameModeOptionsScreen(LcdDisplay& display, GameModeRegistry& gameModesRegistry)
    : display(display), gameModesRegistry(gameModesRegistry) {}

  void onEnter() override {
    selectedSettingIndex = 0;
    fieldFocused = true;
  }

  void render() override {
    display.clear();
    display.showLine(0, "Game Options", "center");

    const GameMode& mode = sessionManager.getSelectedGameMode();
    uint8_t count = mode.getSettingCount();

    for (uint8_t i = 0; i < count && i < 2; ++i) {
      const ModeSetting& setting = mode.getSetting(i);
      String prefix;

      if (fieldFocused && selectedSettingIndex == i) {
        prefix = "> ";
      } else if (!fieldFocused && selectedSettingIndex == i) {
        prefix = "* ";
      } else {
        prefix = "  ";
      }

      String line = prefix + setting.label + ": " + formatValue(setting);
      display.showLine(i + 1, line);  // lines 1 and 2

      ButtonLabels labels = getButtonLabels();
      display.showButtonLabels(labels);
    }
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    GameMode& mode = sessionManager.getMutableSelectedGameMode();

    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
      return;
    }

    if (encoder.wasPressed()) {
      fieldFocused = !fieldFocused;
      render();
      return;
    }

    if (encoder.hasChanged()) {
      int delta = encoder.getDirection();

      if (fieldFocused && mode.getSettingCount() > 0) {
        ModeSetting& setting = mode.getMutableSetting(selectedSettingIndex);
        setting.value = constrain(setting.value + delta, setting.minValue, setting.maxValue);
      } else {
        selectedSettingIndex = constrain(selectedSettingIndex + delta, 0, mode.getSettingCount() - 1);
      }

      render();
      return;
    }

    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Playing);
      return;
    }
  }

  void loop() override {
    // Optional: persist settings or sync with sessionManager
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::AdjustField;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Back", "<>", "Play"};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeOptions;
  }

  String getHash() const override {
    const GameMode& mode = sessionManager.getSelectedGameMode();
    String hash = "GameModeOptions-" + mode.getName();
    for (uint8_t i = 0; i < mode.getSettingCount(); i++) {
      const ModeSetting& setting = mode.getSetting(i);
      hash += "-" + setting.label + ":" + String(setting.value);
    }
    return hash;
  }

private:
  String formatValue(const ModeSetting& setting) const {
    switch (setting.type) {
      case SettingType::TIME:
        return String(setting.value) + " sec";
      case SettingType::HITS:
        return String(setting.value);
        // return String(setting.value) + " hits";
      case SettingType::DIFFICULTY:
        return setting.value == 1 ? "Easy" : (setting.value == 2 ? "Medium" : "Hard");
      default:
        return String(setting.value);
    }
  }

  LcdDisplay& display;
  GameModeRegistry& gameModesRegistry;
  uint8_t selectedSettingIndex = 0;
  bool fieldFocused = false;
};

#endif