#ifndef GAME_MODE_LIST_SCREEN_H
#define GAME_MODE_LIST_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameModeRegistry.h"
#include "../GameSessionManager.h"
#include "../ScreenRenderer.h"
#include "Screen.h"

extern GameSessionManager sessionManager;
extern ScreenRequest request;
extern ScreenRenderer screenRenderer;

// - GameMode List screen:
// I want to show a list of the game modes with their description.
// With the encoder I want to be able to scroll the list vertically with a pointer that shows a currently selected item.
// Based on how many targets are paired the list of game modes changes (for example the user can't choose "battle" if only one target is paired).
// Button left label "Exit": navigates to the "Home screen".
// Button right label "Play" or "Options": if the game mode selected has no extra options the label is "Select" and navigates to the "Playing screen", if the game mode selected needs more options to be set the label is "Options" and navigates to "GameMode Options screen".

class GameModeListScreen : public Screen {
public:
  GameModeListScreen(LcdDisplay& display, GameModeRegistry& gameModesRegistry)
    : display(display), gameModesRegistry(gameModesRegistry) {}

  void onEnter() override {
    uint8_t entityCount = sessionManager.getEntityCount();
    for (uint8_t i = 0; i < GameModeRegistry::MAX_MODES; ++i) {
      const GameMode& mode = gameModesRegistry.getMode(i);
      ModeType type = mode.getType();
      bool isValid = (entityCount == 1 && (type == ModeType::SINGLE || type == ModeType::ALL)) || (entityCount > 1 && (type == ModeType::MULTI || type == ModeType::ALL));
      if (mode.getName() == ModeName::LitTarget) {
        const EntityInfo* entity = sessionManager.getAllEntities();
        uint8_t entityTargetsCount = entity[0].getTargetCount();
        isValid = entityTargetsCount > 1;
      }

      if (isValid) {
        GameMode copy(mode);
        filteredModes[filteredModesCount++] = copy;
      }
    }
  }

  void render() override {
    display.clear();

    if (filteredModesCount == 0) {
      display.showLine(1, "No modes available", "center");
      return;
    }

    const GameMode& mode = filteredModes[currentIndex];
    display.showLine(0, mode.getName(), "center");

    uint8_t entityCount = sessionManager.getEntityCount();
    const String* desc = mode.getDescription().getDescriptionLines(entityCount);
    display.showLine(1, desc[0]);
    display.showLine(2, desc[1]);
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
    if (encoder.hasChanged()) {
      int direction = encoder.getDirection();
      if (direction > 0 && currentIndex < filteredModesCount - 1) {
        currentIndex++;
      }
      if (direction < 0 && currentIndex > 0) {
        currentIndex--;
      }
      screenRenderer.requestRefresh();
    }
    if (right.wasPressed() && filteredModesCount > 0) {
      const GameMode& selectedMode = filteredModes[currentIndex];
      sessionManager.setSelectedGameMode(selectedMode);

      if (selectedMode.getSettingCount() > 0) {
        request = ScreenRequest::to(ScreenType::GameModeOptions);
      } else {
        request = ScreenRequest::to(ScreenType::Playing);
      }
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::SelectItem;
  }

  ButtonLabels getButtonLabels() const override {
    if (filteredModesCount == 0) return {"Exit", "", ""};

    const GameMode& mode = filteredModes[currentIndex];
    return {"Exit", "<>", mode.getSettingCount() > 0 ? "Options" : "Play"};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeList;
  }

  String getHash() const override {
    return "GameModeList-" + String(currentIndex);
  }

private:
  LcdDisplay& display;
  GameModeRegistry& gameModesRegistry;

  GameMode filteredModes[GameModeRegistry::MAX_MODES];
  uint8_t filteredModesCount = 0;
  uint8_t currentIndex = 0;
};

#endif