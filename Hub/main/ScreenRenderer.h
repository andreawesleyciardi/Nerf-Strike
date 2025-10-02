#ifndef SCREEN_RENDERER_H
#define SCREEN_RENDERER_H

#include "DisplayManager.h"
#include "ScreenManager.h"
#include "HubStateManager.h"
#include "GameModeRegistry.h"

class ScreenRenderer {
public:
  ScreenRenderer(DisplayManager& display, ScreenManager& manager, HubStateManager& state, GameModeRegistry& registry);
  void render();

private:
  DisplayManager& display;
  ScreenManager& manager;
  HubStateManager& hubState;
  GameModeRegistry& gameModes;

  void renderHome();
  void renderHelp();
  void renderPairing();
  void renderGameModeList();
};

#endif