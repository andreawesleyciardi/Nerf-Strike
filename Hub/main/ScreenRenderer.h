#ifndef SCREEN_RENDERER_H
#define SCREEN_RENDERER_H

#include "LcdDisplay.h"
#include "ScreenManager.h"
#include "HubStateManager.h"
#include "GameModeRegistry.h"

class ScreenRenderer {
public:
  ScreenRenderer(LcdDisplay& display, ScreenManager& manager, HubStateManager& state, GameModeRegistry& registry);
  void render();

private:
  LcdDisplay& display;
  ScreenManager& manager;
  HubStateManager& hubState;
  GameModeRegistry& gameModes;

  void renderHome();
  void renderHelp();
  void renderPairing();
  void renderGameModeList();
};

#endif