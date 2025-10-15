#ifndef WIN_LOST_SCREEN_H
#define WIN_LOST_SCREEN_H

#include "../LcdDisplay.h"
#include "../ScreenTypes.h"
#include "../EncoderMode.h"
#include "../ButtonLabels.h"
#include "../HubStateManager.h"
#include "../PairingRegistry.h"
#include "../GameLogic.h"
#include "Screen.h"
#include "../GameSessionManager.h"

extern GameSessionManager sessionManager;

class WinLostScreen : public Screen {
public:
  WinLostScreen(LcdDisplay& display, HubStateManager& hubState, PairingRegistry& registry, GameLogic& gameLogic)
    : display(display), hubState(hubState), registry(registry), gameLogic(gameLogic) {}

  void render() override {
    display.clear();
    display.showLine(0, "Game Over");
    display.showLine(1, "You won!");  // You could make this dynamic based on outcome
  }

  void handleInput(RotaryEncoder& encoder, Button& left, Button& right) override {
    if (right.wasPressed()) {
      request = ScreenRequest::to(ScreenType::Home);
    }
  }

  EncoderMode getEncoderMode() const override {
    return EncoderMode::None;
  }

  ButtonLabels getButtonLabels() const override {
    return {"", "", "Home"};
  }

  ScreenType getType() const override {
    return ScreenType::WinLost;
  }

  String getHash() const override {
    String hash = "WinLost-Screen";
    // String hash = "WinLost-" + String((int)hubState.getGameOutcome());
    // for (uint8_t i = 0; i < registry.getCount(); i++) {
    //   uint8_t id = registry.getTargetIdAt(i);
    //   hash += "-" + String(gameLogic.getScoreFor(id));
    // }
    return hash;
  }

private:
  LcdDisplay& display;
  HubStateManager& hubState;
  PairingRegistry& registry;
  GameLogic& gameLogic;
};

#endif