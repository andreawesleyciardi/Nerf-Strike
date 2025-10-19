#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include <Statuses.h>

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"
#include "../GameSessionManager.h"
#include "../GameSession.h"
#include "../ScreenRenderer.h"

extern GameSessionManager sessionManager;
extern ScreenRenderer screenRenderer;

// - Home screen:
// I want to show a message (for now just "Welcome").
// Button left label "Settings": navigates to the "Settings screen".
// Button encoder: starts immediately the game with default session settings.
// Button right label "Start": navigates to the "Pairing screen".

class HomeScreen : public Screen {
public:
  HomeScreen(LcdDisplay& display, PairingRegistry& registry)
   : display(display), registry(registry) {}

  void onEnter() override {
    // ✅ Clear the session
    Serial.println(F("Entered HomeScreen"));
    GameSession defaultSession = createDefaultSession(registry);
    sessionManager.setSession(defaultSession);
    sessionManager.setStatus(GameSessionStatus::Setting, true);
    Serial.print(F("GameSessionStatus: "));
    Serial.println(GameSessionStatusToString(sessionManager.getStatus()));
    screenRenderer.requestRefresh();
  }

  void onExit() override {
    display.clear();
  }

  void render() override {
    display.clear();
    display.showLine(0, "Home screen");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Settings);
    }
    if (encoder.wasPressed()) {
      Serial.println(F("encoder.wasPressed"));
      request = ScreenRequest::to(ScreenType::Playing);
    }
    if (right.wasPressed()) {
      Serial.print(F("right.wasPressed ->GameSessionStatus: "));
      Serial.println(GameSessionStatusToString(sessionManager.getStatus()));
      request = ScreenRequest::to(ScreenType::Entities);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Help", "<>", "Start"};
  }

  ScreenType getType() const override {
    return ScreenType::Home;
  }

  String getHash() const {
    // return "Home-" + String(millis());
    return "Home-" + String(GameSessionStatusToString(sessionManager.getStatus()));
  }

private:
  LcdDisplay& display;
  PairingRegistry& registry;
};

#endif