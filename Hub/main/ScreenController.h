#ifndef SCREEN_CONTROLLER_H
#define SCREEN_CONTROLLER_H

#include "ScreenManager.h"
#include "HubStateManager.h"
#include "PairingRegistry.h"
#include "GameModeRegistry.h"
#include "RotaryEncoder.h"
#include "Button.h"
#include "screens/Screen.h"  // ✅ Needed for ScreenRequest

class ScreenController {
public:
  ScreenController(ScreenManager& sm, HubStateManager& hsm, PairingRegistry& pr, GameModeRegistry& gmr,
                   RotaryEncoder& enc, Button& left, Button& right);
  void update();

private:
  ScreenManager& screenManager;
  HubStateManager& hubState;
  PairingRegistry& registry;
  GameModeRegistry& gameModes;
  RotaryEncoder& encoder;
  Button& leftButton;
  Button& rightButton;

  void handleGameModeList();
};

#endif