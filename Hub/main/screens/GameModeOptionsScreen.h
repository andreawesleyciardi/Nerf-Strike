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
    fieldFocused = false;
  }

  void render() override {
    display.clear();
    const GameMode& mode = sessionManager.getSelectedGameMode();

    if (mode.getSettingCount() == 0) {
      display.showLine(0, "No options");
      display.showLine(1, "");
      return;
    }

    const ModeSetting& setting = mode.getSetting(selectedSettingIndex);
    String label = (fieldFocused ? ">" : "") + setting.label;
    String valueStr = formatValue(setting);

    display.showLine(0, label);
    display.showLine(1, valueStr);
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
        return String(setting.value) + " hits";
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

// #ifndef GAME_MODE_OPTIONS_SCREEN_H
// #define GAME_MODE_OPTIONS_SCREEN_H

// #include "../LcdDisplay.h"
// #include "../ScreenTypes.h"
// #include "../EncoderMode.h"
// #include "../ButtonLabels.h"
// #include "../GameModeRegistry.h"
// #include "Screen.h"
// #include "../GameSessionManager.h"

// extern GameSessionManager sessionManager;

// // - GameMode Options screen:
// // Some game modes will have extra options.
// // Some examples:
// // when the game mode is "To number" the user will be able to, starting from a default number, change the value of the number of hits that has to reach for complete the game but also .
// // when the game mode is "" 
// // Button left label "Back": navigates to the "GameModeList screen".
// // Button encoder: pressed first time focuses on a field so that the value can be changed with the scrolling of the encoder, next press the focus is removed and the scrolling of the encoder is used again for scroll vertically the list of fields.
// // Button right label "Select": navigates to the "GameMode Details screen with the values of the options in memory".

// class GameModeOptionsScreen : public Screen {
// public:
//   GameModeOptionsScreen(LcdDisplay& display, GameModeRegistry& gameModesRegistry)
//     : display(display), gameModesRegistry(gameModesRegistry) {}

//   void render() override {
//     display.clear();
//     display.showLine(0, "Game Options");
//     display.showLine(1, "Adjust settings...");
//   }

//   void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
//     if (left.wasPressed()) {
//       request = ScreenRequest::to(ScreenType::GameModeList);
//     }
//     if (encoder.wasPressed()) {
//       // pressed first time focuses on a field so that the value can be changed with the scrolling of the encoder, next press the focus is removed and the scrolling of the encoder is used again for scroll vertically the list of fields.
//     }
//     if (right.wasPressed()) {
//       request = ScreenRequest::to(ScreenType::Playing);
//     }
//   }

//   void loop() override {
//     // GameMode& mode = sessionManager.getSelectedGameMode();
//     // mode.getSetting(index).value = newValue;
//   }

//   EncoderMode getEncoderMode() const override {
//     return EncoderMode::AdjustField;
//   }

//   ButtonLabels getButtonLabels() const override {
//     return {"Back", "<>", "Play"};
//   }

//   ScreenType getType() const override {
//     return ScreenType::GameModeOptions;
//   }

//   String getHash() const override {
//     const GameMode& mode = gameModesRegistry.getMode(selectedIndex);
//     String hash = "GameModeOptions-" + mode.getName();
//     for (uint8_t i = 0; i < mode.getSettingCount(); i++) {
//       const ModeSetting& setting = mode.getSetting(i);
//       hash += "-" + setting.label + ":" + String(setting.value);
//     }
//     return hash;
//   }

// private:
//   LcdDisplay& display;
//   GameModeRegistry& gameModesRegistry;
//   uint8_t selectedIndex = 0;  // You can update this externally if needed
// };

// #endif