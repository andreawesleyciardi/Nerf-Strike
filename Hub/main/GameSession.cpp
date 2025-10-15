#include "GameSession.h"
#include "GameModeRegistry.h"
#include "GameMode.h"
#include "ColorUtils.h"

extern GameModeRegistry gameModeRegistry;

GameSession createDefaultSession(PairingRegistry& registry) {
  GameSession s;
  s.status = GameSessionStatus::Playing;
  // s.gameMode = gameModeRegistry.getModeByName("Training");
  s.gameMode = gameModeRegistry.getModeByName(ModeName::ToNumber);

  EntityInfo e;
  e.entityId = 0;
  e.type = EntityType::Player;
  e.color = getColorForIndex(0);

  uint8_t pairedIds[MAX_TARGETS_PER_ENTITY];
  uint8_t totalPaired = registry.getPairedTargetCount(pairedIds);

  for (uint8_t i = 0; i < totalPaired && e.targetCount < MAX_TARGETS_PER_ENTITY; ++i) {
    e.addTarget(pairedIds[i]);
    registry.setActive(pairedIds[i], true);
  }

  s.entities[0] = e;
  s.entityCount = 1;

  return s;
}

GameSession getDefaultSession(PairingRegistry& registry) {
  return createDefaultSession(registry);
}