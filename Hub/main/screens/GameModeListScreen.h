#ifndef GAME_MODE_LIST_SCREEN_H
#define GAME_MODE_LIST_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameModeRegistry.h"
#include "Screen.h"
#include "../GameSessionManager.h"

extern GameSessionManager sessionManager;

// - GameMode List screen:
// I want to show a list of the game modes (remember the list we made time ago).
// With the encoder I want to be able to scroll the list vertically with a pointer that shows a currently selected item.
// Based on how many targets are paired the list of game modes changes (for example the user can't choose "battle" if only one target is paired).
// Button left label "Exit": navigates to the "Home screen".
// Button right label "Select" or "Options": if the game mode selected has no extra options the label is "Select" and navigates to the "GameMode Details", if the game mode selected needs more options to be selected the label is "Options" and navigates to "GameMode Options screen".

class GameModeListScreen : public Screen {
public:
  GameModeListScreen(LcdDisplay& display, GameModeRegistry& gameModes)
    : display(display), gameModes(gameModes) {}

  void render() override {
    display.clear();
    display.showLine(0, "Select Game Mode");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeDetails);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::SelectItem;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Exit", "", "Select"};
  }

  ScreenType getType() const override {
    return ScreenType::GameModeList;
  }

  String getHash() const override {
    return "GameModeList";
  }

private:
  LcdDisplay& display;
  GameModeRegistry& gameModes;
};

#endif