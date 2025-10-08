#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Arduino.h>
#include "ScreenTypes.h"
#include "EncoderMode.h"
#include "ButtonLabels.h"
#include "HubStateManager.h"
#include "GameModeRegistry.h"
#include "PairingRegistry.h"
#include "GameLogic.h"
#include "screens/Screen.h"

class ScreenManager {
public:
  ScreenManager(HubStateManager& hubState, GameModeRegistry& modes, PairingRegistry& registry, GameLogic& logic);

  void setup();
  void replace(ScreenType screen);

  ScreenType current() const;
  Screen* getActive() const;
  EncoderMode getEncoderMode() const;
  ButtonLabels getButtonLabels() const;

private:
  void updateContext(ScreenType screen);

  HubStateManager& hubState;
  GameModeRegistry& gameModes;
  PairingRegistry& registry;
  GameLogic& gameLogic;

  static const uint8_t screenCount = 12;
  Screen* screens[screenCount];
  ScreenType activeScreen = ScreenType::Home;

  EncoderMode encoderMode = EncoderMode::None;
  ButtonLabels labels = {"", "", ""};
};

#endif