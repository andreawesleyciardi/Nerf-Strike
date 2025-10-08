#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../GameLogic.h"
#include "../PairingRegistry.h"
#include "Screen.h"

class PlayingScreen : public Screen {
public:
  PlayingScreen(GameLogic& gameLogic, PairingRegistry& registry)
    : gameLogic(gameLogic), registry(registry) {}

  void render(LcdDisplay& display) override {
    display.clear();
    display.showLine(0, "Game in progress");
    display.showLine(1, "Score: 0");  // You can make this dynamic later
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (left.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Confirmation);
    }
    if (encoder.wasPressed()) {
      // SWITCH BETWEEN PAUSE AND PLAY
    }
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Playing);
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
    String hash = "Playing";
    // for (uint8_t i = 0; i < registry.getCount(); i++) {
    //   uint8_t id = registry.getTargetIdAt(i);
    //   hash += "-" + String(gameLogic.getScoreFor(id));
    // }
    return hash;
  }

private:
  GameLogic& gameLogic;
  PairingRegistry& registry;
};

#endif