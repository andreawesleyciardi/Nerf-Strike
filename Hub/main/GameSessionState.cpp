#include "GameSessionState.h"
#include "PairingRegistry.h"
#include "GameModeRegistry.h"

extern GameModeRegistry gameModeRegistry;

GameSessionState::GameSessionState() {
  reset();
}

uint8_t GameSessionState::calculateTargetsPerEntity(uint8_t totalPaired, uint8_t totalPlayers, uint8_t totalTeams) {
  if (totalTeams == 0) {
    // Single player mode
    return (totalPaired - (totalPaired % totalPlayers)) / totalPlayers;
  } else if (totalTeams == 1) {
    // All players on the same team
    return totalPaired;
  }
  // Multiple teams mode
  return (totalPaired - (totalPaired % totalTeams)) / totalTeams;
}

void GameSessionState::initializeDefault(PairingRegistry& registry) {
  reset();
  setSelectedGameMode(gameModeRegistry.getModeByName("Training"));
  status = SessionStatus::Playing;

  uint8_t pairedIds[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
  uint8_t totalPaired = registry.getPairedTargetIds(pairedIds);

  if (totalPaired == 0) {
    Serial.println(F("⚠️ No paired targets found. Skipping entity assignment."));
    return;
  }

  EntityInfo e;
  e.entityId = 0;
  e.type = EntityType::Player;
  e.color = defaultColorFor(0);



  for (uint8_t i = 0; i < totalPaired && e.targetCount < MAX_TARGETS_PER_ENTITY; ++i) {
    e.addTarget(pairedIds[i]);
    registry.setActive(pairedIds[i], true);
  }

  entities[0] = e;
  entityCount = 1;
}

void GameSessionState::reset() {
  for (uint8_t i = 0; i < MAX_ENTITIES; ++i) {
    entities[i].reset();
  }
  entityCount = 0;
  selectedGameMode = GameMode();
  status = SessionStatus::Setting;
}

bool GameSessionState::isMultiplayer() const {
  return entityCount > 1;
}

int GameSessionState::getScoreForEntity(uint8_t entityId) const {
  if (entityId < entityCount) {
    return entities[entityId].score;
  }
  return 0;
}

void GameSessionState::setScoreForEntity(uint8_t entityId, int value) {
  if (entityId < entityCount) {
    entities[entityId].score = value;
  }
}

uint8_t GameSessionState::getEntityIdForTarget(uint8_t targetId) const {
  for (uint8_t i = 0; i < entityCount; ++i) {
    if (entities[i].ownsTarget(targetId)) {
      return entities[i].entityId;
    }
  }
  return 255;  // Invalid
}

void GameSessionState::assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCountInput, bool useTeams, PairingRegistry& registry) {
  entityCount = 0;
  uint8_t targetsPerEntity = calculateTargetsPerEntity(totalTargets)  / entityCountInput;

  for (uint8_t i = 0; i < entityCountInput && entityCount < MAX_ENTITIES; ++i) {
    EntityInfo e;
    e.entityId = i;
    e.type = useTeams ? EntityType::Team : EntityType::Player;
    e.color = defaultColorFor(i);

    for (uint8_t j = 0; j < targetsPerEntity && e.targetCount < MAX_TARGETS_PER_ENTITY; ++j) {
      uint8_t targetIndex = i * targetsPerEntity + j;
      if (targetIndex < totalTargets) {
        uint8_t tid = pairedTargetIds[targetIndex];
        e.addTarget(tid);
        registry.setActive(tid, true);
      }
    }

    entities[entityCount++] = e;
  }

  for (uint8_t i = entityCountInput * targetsPerEntity; i < totalTargets; ++i) {
    registry.setActive(pairedTargetIds[i], false);
  }
}

Color GameSessionState::defaultColorFor(uint8_t index) const {
  uint8_t colorIndex = index % (sizeof(EntityColorSequence) / sizeof(Color));
  return EntityColorSequence[colorIndex];
}

// void GameSessionState::defaultState(PairingRegistry& registry) {
//   reset();
//   selectedGameMode = gameModeRegistry.getModeByName("Training");
//   status = SessionStatus::Playing;

//   uint8_t pairedIds[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
//   uint8_t totalPaired = registry.getPairedTargetIds(pairedIds);

//   EntityInfo e;
//   e.entityId = 0;
//   e.type = EntityType::Player;
//   e.color = Color::Cyan;

//   for (uint8_t i = 0; i < totalPaired && e.targetCount < MAX_TARGETS_PER_ENTITY; ++i) {
//     e.addTarget(pairedIds[i]);
//     registry.setActive(pairedIds[i], true);
//   }

//   entities[0] = e;
//   entityCount = 1;
// }

void GameSessionState::setSelectedGameMode(const GameMode& mode) {
  selectedGameMode = mode;
}

const GameMode& GameSessionState::getSelectedGameMode() const {
  return selectedGameMode;
}

void GameSessionState::restart() {
  for (uint8_t i = 0; i < entityCount; ++i) {
    entities[i].score = 0;
  }
  status = SessionStatus::Playing;
}