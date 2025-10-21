#ifndef PAIRING_SCREEN_H
#define PAIRING_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../PairingRegistry.h"
#include "../GameSessionManager.h"
#include "Screen.h"

extern GameSessionManager sessionManager;
extern ScreenRequest request;
extern ScreenRenderer screenRenderer;

// - Pairing screen:
// I want to show the message "Pairing…" if the pairing with the targets is on going (if possible show the text with some animation like pulse for example for make clear to the user that the Hub is working and not blocked).
// When the pairing is finish I want to show "[x] Targets were found" and [x] is the number of paired targets.
// Button left label "Targets": navigates to the "Target List screen".
// Button right label "Play": navigates to the "GameMode List screen".

class PairingScreen : public Screen {
public:
  PairingScreen(LcdDisplay& display, PairingRegistry& registry)
    : display(display), registry(registry) {}

  void onEnter() override {
    scrollIndex = 0;
    screenRenderer.requestRefresh();
  }

  void render() override {
    display.clear();
    uint8_t targetsCount = registry.getPairedTargetCount();
    String targetsCountFormatted = targetsCount < 10 ? "0" + String(targetsCount) : String(targetsCount);
    display.showLine(1, "Paired Targets (" + targetsCountFormatted + "):", "center");

    const uint8_t* ids = registry.getAllPairedTargetIds();

    if (targetsCount == 0) {
      // Manual dot animation (Arduino String has no .repeat())
      uint8_t dotCount = (millis() / 500) % 4;
      String dots = "";
      for (uint8_t i = 0; i < dotCount; ++i) {
        dots += ".";
      }
      display.showLine(2, "Searching" + dots, "center");
      display.showLine(3, "", "center");
    } else {
      if (scrollIndex >= targetsCount) scrollIndex = targetsCount - 1;

      for (uint8_t i = 0; i < 2; ++i) {
        uint8_t idx = scrollIndex + i;
        if (idx < targetsCount) {
          String label = "Target " + String(ids[idx]);
          display.showLine(2 + i, label);
        } else {
          display.showLine(2 + i, "");
        }
      }
    }
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }

    uint8_t targetsCount = registry.getPairedTargetCount();
    if (encoder.hasChanged() && targetsCount > 2) {
      int delta = encoder.getDelta();
      scrollIndex = constrain(scrollIndex + delta, 0, targetsCount - 2);
      screenRenderer.requestRefresh();
    }

    if (right.wasPressed()) {
      Serial.println(F("New Pairing"));  // Placeholder for future broadcast
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::SelectItem;
  }

  ButtonLabels getButtonLabels() const override {
    return {"Home", "", "Pair"};
  }

  ScreenType getType() const override {
    return ScreenType::Pairing;
  }

  String getHash() const override {
    return "Pairing-" + String(scrollIndex);
  }

private:
  LcdDisplay& display;
  PairingRegistry& registry;
  uint8_t scrollIndex = 0;
};

#endif