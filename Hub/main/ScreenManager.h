#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Arduino.h>
#include "LcdDisplay.h"
#include "ScreenTypes.h"
#include "EncoderMode.h"
// #include "ButtonLabels.h"
#include "HubStateManager.h"
#include "GameModeRegistry.h"
#include "PairingRegistry.h"
#include "GameLogic.h"
#include "Communication.h"
#include "screens/Screen.h"

class ScreenManager {
public:
  ScreenManager(LcdDisplay& display, HubStateManager& hubState, GameModeRegistry& modes,
                PairingRegistry& registry, GameLogic& logic, Communication& communication);

  void setup();
  void replace(ScreenType screen);

  ScreenType current() const;
  Screen* getActive() const;
  EncoderMode getEncoderMode() const;
  // ButtonLabels getButtonLabels() const;

private:
  void updateContext(ScreenType screen);
  void destroyScreens();

  LcdDisplay& display;
  HubStateManager& hubState;
  GameModeRegistry& gameModesRegistry;
  PairingRegistry& registry;
  GameLogic& gameLogic;
  Communication& communication;

  static const uint8_t screenCount = 15;
  Screen* screens[screenCount] = {nullptr};
  ScreenType activeScreen = ScreenType::Splash;

  EncoderMode encoderMode = EncoderMode::None;
  // ButtonLabels labels = {"", "", ""};
};

#endif