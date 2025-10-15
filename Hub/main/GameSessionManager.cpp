#include "GameSessionManager.h"
#include "GameModeRegistry.h"
#include "EntityInfo.h"
#include "ColorUtils.h"

extern GameModeRegistry gameModeRegistry;

GameSessionManager::GameSessionManager() {
  session.reset();
}

// void GameSessionManager::initializeDefault(PairingRegistry& registry) {
//   session.reset();
//   setSelectedGameMode(gameModeRegistry.getModeByName("ToNumber"));
//   // setSelectedGameMode(gameModeRegistry.getModeByName("Training"));
//   session.status = GameSessionStatus::Playing;

//   uint8_t pairedIds[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
//   uint8_t totalPaired = registry.getPairedTargetCount(pairedIds);

//   if (totalPaired == 0) {
//     Serial.println(F("⚠️ No paired targets found. Skipping entity assignment."));
//     return;
//   }

//   EntityInfo e;
//   e.entityId = 0;
//   e.type = EntityType::Player;
//   e.color = getColorForIndex(0);

//   for (uint8_t i = 0; i < totalPaired && e.targetCount < MAX_TARGETS_PER_ENTITY; ++i) {
//     e.addTarget(pairedIds[i]);
//     registry.setActive(pairedIds[i], true);
//   }

//   session.entities[0] = e;
//   session.entityCount = 1;
// }

void GameSessionManager::assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCountInput, bool useTeams, PairingRegistry& registry) {
  session.entityCount = 0;
  uint8_t targetsPerEntity = calculateTargetsPerEntity(totalTargets, entityCountInput, useTeams ? entityCountInput : 0);

  for (uint8_t i = 0; i < entityCountInput && session.entityCount < MAX_ENTITIES; ++i) {
    EntityInfo e;
    e.entityId = i;
    e.type = useTeams ? EntityType::Team : EntityType::Player;
    e.color = getColorForIndex(i);

    for (uint8_t j = 0; j < targetsPerEntity && e.targetCount < MAX_TARGETS_PER_ENTITY; ++j) {
      uint8_t targetIndex = i * targetsPerEntity + j;
      if (targetIndex < totalTargets) {
        uint8_t tid = pairedTargetIds[targetIndex];
        e.addTarget(tid);
        registry.setActive(tid, true);
      }
    }

    session.entities[session.entityCount++] = e;
  }

  for (uint8_t i = entityCountInput * targetsPerEntity; i < totalTargets; ++i) {
    registry.setActive(pairedTargetIds[i], false);
  }
}

uint8_t GameSessionManager::calculateTargetsPerEntity(uint8_t totalPaired, uint8_t totalPlayers, uint8_t totalTeams) const {
  if (totalTeams == 0) {
    return (totalPaired - (totalPaired % totalPlayers)) / totalPlayers;
  } else if (totalTeams == 1) {
    return totalPaired;
  }
  return (totalPaired - (totalPaired % totalTeams)) / totalTeams;
}

bool GameSessionManager::isMultiplayer() const {
  return session.entityCount > 1;
}

int GameSessionManager::getScoreForEntity(uint8_t entityId) const {
  Serial.print(F("Inside GETScoreForEntity. entityId: "));
  Serial.print(entityId);
  Serial.print(F("Inside GETScoreForEntity. session.entityCount: "));
  Serial.print(session.entityCount);
  if (entityId < session.entityCount) {
    Serial.print(F("Inside IF in GETScoreForEntity. session.entities[entityId].score: "));
    Serial.print(session.entities[entityId].score);
    return session.entities[entityId].score;
  }
  return 0;
}

void GameSessionManager::setScoreForEntity(uint8_t entityId, int value) {
  Serial.print(F("Inside SETScoreForEntity. entityId: "));
  Serial.print(entityId);
  Serial.print(F("Inside SETScoreForEntity. value: "));
  Serial.print(value);
  Serial.print(F("Inside SETScoreForEntity. session.entityCount: "));
  Serial.print(session.entityCount);
  if (entityId < session.entityCount) {
    session.entities[entityId].score = value;
  }
}

uint8_t GameSessionManager::getEntityIdForTarget(uint8_t targetId) const {
  for (uint8_t i = 0; i < session.entityCount; ++i) {
    if (session.entities[i].ownsTarget(targetId)) {
      return session.entities[i].entityId;
    }
  }
  return 255;
}

const EntityInfo (&GameSessionManager::getAllEntities() const)[MAX_ENTITIES] {
  return session.entities;
}

uint8_t GameSessionManager::getEntityCount() const {
  return session.entityCount;
}

void GameSessionManager::setEntityCount(uint8_t value) {
  session.entityCount = value;
}

void GameSessionManager::setSelectedGameMode(const GameMode& mode) {
  session.gameMode = mode;
}

const GameMode& GameSessionManager::getSelectedGameMode() const {
  return session.gameMode;
}

void GameSessionManager::setSession(const GameSession& newSession) {
  session.reset();
  session = newSession;
}

GameSession& GameSessionManager::getSession() {
  return session;
}

const GameSession& GameSessionManager::getSession() const {
  return session;
}

void GameSessionManager::restart() {
  for (uint8_t i = 0; i < session.entityCount; ++i) {
    session.entities[i].score = 0;
  }
  setStatus(GameSessionStatus::Playing);
}
void GameSessionManager::setStatus(GameSessionStatus newStatus) {
  session.status = newStatus;
}