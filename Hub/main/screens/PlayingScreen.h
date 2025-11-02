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

    // ✅ Start countdown
    countdownStarted = false;
    countdownActive = true;
    countdownValue = 5;
    countdownStartTime = millis();
    timeDisplay.showScore(countdownValue);
    screenRenderer.requestRefresh();

    // Reset game timer
    gameTimerActive = false;
    gameTimerDuration = 0;
    gameTimerStartTime = 0;
  }

  void onExit() override {
    timeDisplay.clear();
  }

  void render() override {
    display.clear();
    if (!countdownStarted) {
      display.showLine(1, "Ready...", "center");
      delay(1000);
      countdownStarted = true;
    } else if (countdownActive) {
      display.showLine(0, "Starting in...", "center");
      display.showLine(1, "0" + String(countdownValue), "center");
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
        // Toggle pause/play
        if (currentStatus == GameSessionStatus::Playing) {
          sessionManager.setStatus(GameSessionStatus::Paused, true);
          // ✅ Pause game timer
          if (gameTimerActive) {
            gameTimerPaused = true;
            gameTimerPauseTime = millis();
          }
        }
        else if (currentStatus == GameSessionStatus::Paused) {
          sessionManager.setStatus(GameSessionStatus::Playing, true);
          // ✅ Resume game timer
          if (gameTimerActive && gameTimerPaused) {
            unsigned long pausedDuration = millis() - gameTimerPauseTime;
            gameTimerStartTime += pausedDuration;
            gameTimerPaused = false;
          }
        }
      }
    }
    if (right.wasPressed()) {
      sessionManager.restart();
      request = ScreenRequest::to(ScreenType::Playing);
    }
  }

  void loop() override {
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
          sessionManager.setStatus(GameSessionStatus::Playing, true);
          screenRenderer.requestRefresh();

          // ✅ Start game timer if TIME setting exists
          const GameMode& gameMode = sessionManager.getSelectedGameMode();
          const ModeSetting* settings = gameMode.getAllSettings();
          for (uint8_t i = 0; i < gameMode.getSettingCount(); ++i) {
            if (settings[i].type == SettingType::TIME) {
              gameTimerDuration = settings[i].value * 1000UL;  // convert to ms
              gameTimerStartTime = millis();
              gameTimerActive = true;
              break;
            }
          }
        }
      }
    }

    // ✅ Check game timer
    if (gameTimerActive && sessionManager.getStatus() == GameSessionStatus::Playing) {
      unsigned long now = millis();
      if (now - gameTimerStartTime >= gameTimerDuration) {
        gameTimerActive = false;
        ScoreUpdateBatch batch = gameLogic.gameTimerEnded();
        if (communication.alertGameTimerEnded(batch)) {
          Serial.println(F("✅ Alerted targets of timer game ended"));
        }
        else {
          Serial.println(F("❌ Not alerted targets of timer game ended"));
        }
      }
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
    if (!countdownStarted) return "Playing-ready";
    if (countdownActive) return "Playing-countdown-" + String(countdownValue);
    return "Playing-active";
  }

private:
  LcdDisplay& display;
  GameLogic& gameLogic;
  Communication& communication;

  bool countdownStarted = false;
  bool countdownActive = false;
  uint8_t countdownValue = 0;
  unsigned long countdownStartTime = 0;

  // ✅ Game timer fields
  bool gameTimerActive = false;
  unsigned long gameTimerStartTime = 0;
  unsigned long gameTimerDuration = 0;

  bool gameTimerPaused = false;
  unsigned long gameTimerPauseTime = 0;
};

#endif