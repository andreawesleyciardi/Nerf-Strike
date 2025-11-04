#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include <SevenSegmentDisplay.h>

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameLogic.h"
#include "../GameSessionManager.h"
#include "../HubPins.h"
#include "../ScreenRenderer.h"
#include "Screen.h"

extern GameSessionManager sessionManager;
extern ScreenRequest request;
extern SevenSegmentDisplay timeDisplay;
extern ScreenRenderer screenRenderer;

class PlayingScreen : public Screen {
public:
  PlayingScreen(LcdDisplay& display, GameLogic& gameLogic, Communication& communication)
    : display(display), gameLogic(gameLogic), communication(communication) {}

  void onEnter() override {
    sessionManager.communicateTargetSessionInfo();
    sessionManager.setStatus(GameSessionStatus::Starting, true);

    countdown = {};
    countdown.active = true;
    countdown.value = 5;
    countdown.startTime = millis();
    timeDisplay.showScore(countdown.value);
    screenRenderer.requestRefresh();

    sessionManager.getTimerState() = {};
    sessionManager.getLitTargetState() = {};
  }

  void onExit() override {
    timeDisplay.clear();
  }

  void render() override {
    display.clear();
    if (!countdown.started) {
      display.showLine(1, "Ready...", "center");
      delay(1000);
      countdown.started = true;
    } else if (countdown.active) {
      display.showLine(0, "Starting in...", "center");
      display.showLine(1, "0" + String(countdown.value), "center");
    } else if (sessionManager.getStatus() == GameSessionStatus::Paused) {
      display.showLine(1, "Paused", "center");
    } else if (sessionManager.getStatus() == GameSessionStatus::Ended) {
      display.showLine(1, "Finished", "center");
    } else {
      display.showLine(0, "Game in", "center");
      display.showLine(1, "progress...", "center");
    }
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      Serial.println(F("GameSessionStatus::Ended -> Going to Home"));
      sessionManager.setStatus(GameSessionStatus::Ended, true);
      request = ScreenRequest::to(ScreenType::Home);
    }
    if (encoder.wasPressed()) {
      GameSessionStatus currentStatus = sessionManager.getStatus();
      if (currentStatus == GameSessionStatus::Playing || currentStatus == GameSessionStatus::Paused) {
        if (currentStatus == GameSessionStatus::Playing) {
          sessionManager.setStatus(GameSessionStatus::Paused, true);
          gameLogic.timerPause();
        } else {
          sessionManager.setStatus(GameSessionStatus::Playing, true);
          gameLogic.timerResume();
        }
      }
    }
    if (right.wasPressed()) {
      sessionManager.restart();
      request = ScreenRequest::to(ScreenType::Playing);
    }
  }

  void loop() override {
    unsigned long now = millis();

    const GameMode& gameMode = sessionManager.getSelectedGameMode();
    String gameModeName = gameMode.getName();

    if (countdown.active && now - countdown.startTime >= 1000) {
      countdown.startTime = now;
      countdown.value--;

      if (countdown.value > 0) {
        timeDisplay.showScore(countdown.value);
        screenRenderer.requestRefresh();
      } else {
        timeDisplay.clear();
        countdown.active = false;
        sessionManager.setStatus(GameSessionStatus::Playing, true);
        screenRenderer.requestRefresh();
        
        if (gameModeName.equals(ModeName::Timer)) {
          gameLogic.timerSetup();
        }
        if (gameModeName.equals(ModeName::LitTarget)) {
          gameLogic.litTargetSetup();
        }
      }
    }

    if (gameModeName.equals(ModeName::Timer)) {
      gameLogic.timerLoop();
    }
    if (gameModeName.equals(ModeName::LitTarget)) {
      gameLogic.litTargetLoop();
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Exit", sessionManager.getStatus() == GameSessionStatus::Ended ? "" : "<>", "Reset"};
  }

  ScreenType getType() const override {
    return ScreenType::Playing;
  }

  String getHash() const override {
    if (sessionManager.getStatus() == GameSessionStatus::Paused) return "Paused";
    if (sessionManager.getStatus() == GameSessionStatus::Ended) return "Ended";
    if (!countdown.started) return "Playing-ready";
    if (countdown.active) return "Playing-countdown-" + String(countdown.value);
    return "Playing-active";
  }

private:
  LcdDisplay& display;
  GameLogic& gameLogic;
  Communication& communication;

  struct CountdownState {
    bool started = false;
    bool active = false;
    uint8_t value = 0;
    unsigned long startTime = 0;
  } countdown;
};

#endif