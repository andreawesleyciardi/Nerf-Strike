#ifndef ENTITIES_SCREEN_H
#define ENTITIES_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "Screen.h"
#include "../GameSessionManager.h"
#include "../GameSession.h"

extern GameSessionManager sessionManager;

// - Entities screen:
// The user can decide the number of players/teams (entities).
// Button left label "Pair": navigates to the "Pairing screen".
// Scroll encoder: changes the number of entities.
// Button right label "Next": navigates to the "Game Mode List screen".

class EntitiesScreen : public Screen {
public:
  EntitiesScreen(LcdDisplay& display, PairingRegistry& registry)
   : display(display), registry(registry) {}

  void onEnter() override {
    Serial.println("Entered the EntitiesScreen");
  }

  void render() override {
    display.clear();
    display.showLine(0, "Choose the number of", "center");
    display.showLine(1, "Players / Teams:", "center");
    uint8_t count = sessionManager.getEntityCount();
    String formatted = count < 10 ? "0" + String(count) : String(count);
    display.showLine(2, formatted, "center");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Pairing);
    }
    if (encoder.hasChanged()) {
      Serial.println("encoder has changed");
      int delta = encoder.getDelta();  // +1 or -1 depending on rotation
      uint8_t current = sessionManager.getEntityCount();
      uint8_t pairedIds[MAX_TARGETS_PER_ENTITY];
      uint8_t maxEntities = registry.getPairedTargetCount(pairedIds);  // You need this method
      uint8_t updated = constrain(current + delta, 1, maxEntities);
      Serial.print("updated value: ");
      Serial.println(updated);
      sessionManager.setEntityCount(updated);
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::NavigateButtons;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Pair", "<>", "Next"};
  }

  ScreenType getType() const override {
    return ScreenType::Entities;
  }

  String getHash() const {
  return "Entities_" + String(sessionManager.getEntityCount());
}

private:
  LcdDisplay& display;
  PairingRegistry& registry;
};

#endif