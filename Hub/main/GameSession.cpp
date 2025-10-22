#include "GameSession.h"
#include "GameModeRegistry.h"
#include "GameMode.h"
#include "ColorUtils.h"

extern GameModeRegistry gameModeRegistry;

GameSession createDefaultSession(PairingRegistry& registry) {
  GameSession newSession;
  newSession.status = GameSessionStatus::Setting;
  newSession.gameMode = gameModeRegistry.getModeByName("Training");

  EntityInfo newEntity;
  newEntity.entityId = 0;
  newEntity.type = EntityType::Player;
  newEntity.setColor(0);

  // Assignes all the targets to the only entity
    uint8_t totalPaired = registry.getPairedTargetCount();
    const uint8_t* pairedTargetIds = registry.getAllPairedTargetIds();
    for (uint8_t i = 0; i < totalPaired && newEntity.targetCount < MAX_TARGETS_PER_ENTITY; ++i) {
      newEntity.addTarget(pairedTargetIds[i]);
      registry.setActive(pairedTargetIds[i], true);
    }

  newSession.entities[0] = newEntity;
  newSession.entityCount = 1;

  return newSession;
}

GameSession getDefaultSession(PairingRegistry& registry) {
  return createDefaultSession(registry);
}