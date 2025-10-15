#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include <SevenSegmentDisplay.h>

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameLogic.h"
#include "../PairingRegistry.h"
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
  PlayingScreen(LcdDisplay& display, GameLogic& gameLogic, PairingRegistry& registry, Communication& communication)
    : display(display), gameLogic(gameLogic), registry(registry), communication(communication) {}

  void onEnter() override {
    // ✅ Push entity colors to targets
    GameSession& session = sessionManager.getSession();
    Serial.print(F("session.getSelectedGameMode().getName(): "));
    Serial.println(sessionManager.getSelectedGameMode().getName());
    
    Serial.println(F("Playing onEnter"));
    Serial.print(F("session.entityCount: "));
    Serial.println(session.entityCount);
    for (uint8_t i = 0; i < session.entityCount; ++i) {
      const EntityInfo& entity = session.entities[i];
      const Color& color = entity.color;
      Serial.print(F("entity.targetCount: "));
      Serial.println(entity.targetCount);
      for (uint8_t j = 0; j < entity.targetCount; ++j) {
        uint8_t targetId = entity.targetIds[j];
        Serial.print(F("Sending entity color "));
        Serial.print(color.name);
        Serial.print(F(" to targetId "));
        Serial.println(targetId);

        char colorName[16];
        strncpy(colorName, color.name.c_str(), sizeof(colorName));
        colorName[sizeof(colorName) - 1] = '\0';
        communication.entityColor(targetId, colorName);
      }
    }

    // ✅ Start countdown
    countdownStarted = true;
    countdownActive = true;
    countdownValue = 5;
    countdownStartTime = millis();
    timeDisplay.showScore(countdownValue);
    screenRenderer.requestRefresh();
  }

  void render() override {
    display.clear();
    if (!countdownStarted) {
      display.showLine(1, "Ready...", "center");
    } else if (countdownActive) {
      display.showLine(1, "Starting in...", "center");
    } else {
      display.showLine(1, "Game in progress", "center");
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
  Communication& communication;

  bool countdownStarted = false;
  bool countdownActive = false;
  uint8_t countdownValue = 0;
  unsigned long countdownStartTime = 0;
};

#endif