#include "ScreenController.h"
#include "ScreenManager.h"
#include "screens/Screen.h"

ScreenController::ScreenController(ScreenManager& sm, HubStateManager& hsm, PairingRegistry& pr, GameModeRegistry& gmr,
                                   RotaryEncoder& enc, Button& left, Button& right)
  : screenManager(sm), hubState(hsm), registry(pr), gameModes(gmr),
    encoder(enc), leftButton(left), rightButton(right) {}

void ScreenController::update() {
  ScreenType current = screenManager.current();

  // General input handling via active screen
  Screen* activeScreen = screenManager.getActive();
  if (activeScreen) {
    activeScreen->handleInput(encoder, leftButton, rightButton, screenManager);
  }

  // Optional: screen-specific overrides
  if (current == ScreenType::GameModeList) {
    handleGameModeList();  // Custom logic layered on top
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
    screenManager.push(ScreenType::GameModeOptions);
  }

  if (leftButton.wasPressed()) {
    screenManager.pop();
  }
}