#include "ScreenController.h"

ScreenController::ScreenController(ScreenManager& sm, HubStateManager& hsm, PairingRegistry& pr, GameModeRegistry& gmr,
                                   RotaryEncoder& enc, Button& left, Button& right)
  : screenManager(sm), hubState(hsm), registry(pr), gameModes(gmr),
    encoder(enc), leftButton(left), rightButton(right) {}

void ScreenController::update() {
  ScreenID current = screenManager.current();

  if (current == ScreenID::GameModeList) {
    handleGameModeList();
  }

  // Add other screen-specific handlers here
}

void ScreenController::handleGameModeList() {
  int dir = encoder.getDirection();
  if (dir != 0) {
    if (dir > 0) hubState.nextMenuItem();
    else hubState.previousMenuItem();
  }

  if (encoder.wasPressed() || rightButton.wasPressed()) {
    const GameMode& selected = gameModes.getMode(hubState.getCurrentMenuIndex());
    Serial.print("🎮 Selected Game Mode: ");
    Serial.println(selected.getName());
    screenManager.push(ScreenID::GameModeOptions);
  }

  if (leftButton.wasPressed()) {
    screenManager.pop();
  }
}