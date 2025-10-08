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
    activeScreen->handleInput(encoder, leftButton, rightButton);

    // Check if the screen requested a transition
    ScreenRequest request = activeScreen->getRequestedScreen();
    if (request.hasRequest) {
      screenManager.replace(request.target);
    }
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

    // Instead of pushing, we now request a screen change
    screenManager.replace(ScreenType::GameModeOptions);
  }

  if (leftButton.wasPressed()) {
    screenManager.replace(ScreenType::Home);  // Or wherever "back" should go
  }
}