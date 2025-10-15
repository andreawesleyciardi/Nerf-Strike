#ifndef TARGET_LIST_SCREEN_H
#define TARGET_LIST_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../PairingRegistry.h"
#include "Screen.h"
#include "../GameSessionManager.h"

extern GameSessionManager sessionManager;

// - Target List screen:
// I see a list of the targets paired (Target 1, Target 2, Target 3…) and next to each one their ID.
// With the encoder I want to be able to scroll the list vertically with a pointer that shows a currently selected item.
// Button left label "Re-Pair": navigates to the "Pairing screen" and tries to pair again with the targets.
// Button encoder: sends a blink command (OPCODE_BLINK_COMMAND) to the currently selected target. Would be nice if the target replies to the Blink command sending to the hub it's battery level so can be shown next to the ID.
// Button right label "Play": navigates to the "GameMode List screen".

class TargetListScreen : public Screen {
public:
  TargetListScreen(LcdDisplay& display, PairingRegistry& registry)
    : display(display), registry(registry) {}

  void render() override {
    display.clear();
    display.showLine(0, "Target List");
    display.showLine(1, "Paired targets: XX");
    // display.showLine(1, "Paired targets: " + String(registry.getCount()));
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Pairing);
    }
    if (encoder.wasPressed()) {
      // ends a blink command (OPCODE_BLINK_COMMAND) to the currently selected target. Would be nice if the target replies to the Blink command sending to the hub it's battery level so can be shown next to the ID.
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::GameModeList);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::VerticalScroll;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Re-pair", "", "Play"};
  }

  ScreenType getType() const override {
    return ScreenType::TargetList;
  }

  String getHash() const override {
    String hash = "TargetList";
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