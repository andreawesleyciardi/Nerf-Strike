#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include <SevenSegmentDisplay.h>

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameLogic.h"
#include "../PairingRegistry.h"
#include "../GameSessionState.h"
#include "../HubPins.h"
#include "../ScreenRenderer.h"
#include "Screen.h"

extern GameSessionState session;
extern ScreenRequest request;
extern SevenSegmentDisplay timeDisplay;
extern ScreenRenderer screenRenderer;

class PlayingScreen : public Screen {
public:
  PlayingScreen(LcdDisplay& display, GameLogic& gameLogic, PairingRegistry& registry)
    : display(display), gameLogic(gameLogic), registry(registry) {}

  void render() override {
    display.clear();
    if (!countdownStarted) {
      display.showLine(1, "Ready...", "center");
    } else if (countdownActive) {
      display.showLine(1, "Starting in...", "center");
    } else {
      display.showLine(1, "Game in progress", "center");
      // display.showLine(1, "Score: 0");  // You can make this dynamic later
    }
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Confirmation);
    }
    if (encoder.wasPressed()) {
      // TODO: toggle pause/play
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Playing);
    }
  }

  void loop() override {
    if (!countdownStarted) {
      countdownStarted = true;
      countdownActive = true;
      countdownValue = 5;
      countdownStartTime = millis();
      screenRenderer.requestRefresh();  // Trigger LCD update
      timeDisplay.showScore(countdownValue);
    }

    if (countdownActive) {
      unsigned long now = millis();
      if (now - countdownStartTime >= 1000) {
        countdownStartTime = now;
        countdownValue--;

        if (countdownValue > 0) {
          timeDisplay.showScore(countdownValue);
          screenRenderer.requestRefresh();
        } else {
          timeDisplay.clear();
          countdownActive = false;
          screenRenderer.requestRefresh();
        }
      }
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Stop", "<>", "Restart"};
  }

  ScreenType getType() const override {
    return ScreenType::Playing;
  }

  String getHash() const override {
    if (!countdownStarted) return "Playing-ready";
    if (countdownActive) return "Playing-countdown-" + String(countdownValue);
    return "Playing-active";
  }

private:
  LcdDisplay& display;
  GameLogic& gameLogic;
  PairingRegistry& registry;

  bool countdownStarted = false;
  bool countdownActive = false;
  uint8_t countdownValue = 0;
  unsigned long countdownStartTime = 0;
};

#endif