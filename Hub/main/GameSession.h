#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include <Arduino.h>
#include <Statuses.h>
#include "EntityInfo.h"
#include "GameMode.h"
#include "GameModeRegistry.h"
#include "ColorUtils.h"
#include "PairingRegistry.h"

#define MAX_ENTITIES 10

struct GameSession {
  GameSessionStatus status = GameSessionStatus::Setting;
  GameMode gameMode;
  EntityInfo entities[MAX_ENTITIES];
  uint8_t entityCount = 0;

  void reset() {
    for (uint8_t i = 0; i < MAX_ENTITIES; ++i) {
      entities[i].reset();
    }
    entityCount = 0;
    gameMode = GameMode();
    status = GameSessionStatus::Setting;
  }
};

GameSession createDefaultSession(PairingRegistry& registry);
GameSession getDefaultSession(PairingRegistry& registry);

#endif