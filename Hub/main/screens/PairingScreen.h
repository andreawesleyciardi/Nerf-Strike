#ifndef PAIRING_SCREEN_H
#define PAIRING_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../PairingRegistry.h"
#include "Screen.h"

// - Pairing screen:
// I want to show the message "Pairing…" if the pairing with the targets is on going (if possible show the text with some animation like pulse for example for make clear to the user that the Hub is working and not blocked).
// When the pairing is finish I want to show "[x] Targets were found" and [x] is the number of paired targets.
// Button left label "Targets": navigates to the "Target List screen".
// Button right label "Play": navigates to the "GameMode List screen".
#include "../GameSessionState.h"

extern GameSessionState session;

class PairingScreen : public Screen {
public:
  PairingScreen(LcdDisplay& display, PairingRegistry& registry)
    : display(display), registry(registry) {}

  void render() override {
    display.clear();
    display.printAligned("Pairing...", 1, "center");
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::TargetList);
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Targets", "", "Next"};
  }

  ScreenType getType() const override {
    return ScreenType::Pairing;
  }

  String getHash() const override {
    String hash = "Pairing";
    // for (uint8_t i = 0; i < registry.getCount(); i++) {
    //   hash += "-" + String(registry.getTargetIdAt(i));
    //   hash += ":" + String((int)registry.getTargetTypeAt(i));
    // }
    return hash;
  }

private:
  LcdDisplay& display;
  PairingRegistry& registry;
};

#endif