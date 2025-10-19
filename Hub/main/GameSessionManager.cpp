#include "GameSessionManager.h"
#include "GameModeRegistry.h"
#include "EntityInfo.h"
#include "ColorUtils.h"
#include "Communication.h"

extern GameModeRegistry gameModeRegistry;
extern Communication communication;

GameSessionManager::GameSessionManager(PairingRegistry& registry) :
  registry(registry) {
    session.reset();
  }

void GameSessionManager::assignEntitiesBalanced(bool useTeams) {
  uint8_t totalTargets = registry.getPairedTargetCount();
  const uint8_t* pairedTargetIds = registry.getAllPairedTargetIds();

  // Ensure entity count is within bounds
  if (session.entityCount == 0 || session.entityCount > totalTargets) {
    session.entityCount = 1;  // fallback to minimum valid value
  }

  uint8_t targetsPerEntity = calculateTargetsPerEntity(totalTargets, session.entityCount);
  uint8_t targetIndex = 0;

  for (uint8_t entityIndex = 0; entityIndex < session.entityCount && entityIndex < MAX_ENTITIES; ++entityIndex) {
    EntityInfo newEntity;
    newEntity.entityId = entityIndex;
    newEntity.type = useTeams ? EntityType::Team : EntityType::Player;
    newEntity.color = getColorForIndex(entityIndex);

    for (uint8_t j = 0; j < targetsPerEntity && newEntity.targetCount < MAX_TARGETS_PER_ENTITY; ++j) {
      if (targetIndex < totalTargets) {
        uint8_t targetId = pairedTargetIds[targetIndex++];
        newEntity.addTarget(targetId);
        registry.setActive(targetId, true);
      }
    }

    session.entities[entityIndex] = newEntity;
  }

  // Deactivate leftover targets
  for (; targetIndex < totalTargets; ++targetIndex) {
    registry.setActive(pairedTargetIds[targetIndex], false);
    // TODO: Send OPCODE_TARGET_DISABLE to these targets
  }
}

uint8_t GameSessionManager::calculateTargetsPerEntity(uint8_t totalPaired, uint8_t totalEntities) const {
  if (totalEntities == 0) return 0;
  return (totalPaired - (totalPaired % totalEntities)) / totalEntities;
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
  Serial.print(F(" - value: "));
  Serial.print(value);
  Serial.print(F(" - session.entityCount: "));
  Serial.println(session.entityCount);
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
  // session.reset();
  session = newSession;
}

GameSession& GameSessionManager::getSession() {
  return session;
}

const GameSession& GameSessionManager::getSession() const {
  return session;
}

void GameSessionManager::resetSession() {
  session.reset();
}

void GameSessionManager::restart() {
  for (uint8_t i = 0; i < session.entityCount; ++i) {
    session.entities[i].score = 0;
  }
  setStatus(GameSessionStatus::Playing);
}

void GameSessionManager::communicateEntityColor(uint8_t targetId) {
  for (uint8_t i = 0; i < session.entityCount; ++i) {
    const EntityInfo& entity = session.entities[i];
    const Color& color = entity.color;
    for (uint8_t j = 0; j < entity.targetCount; ++j) {
      uint8_t currentTargetId = entity.targetIds[j];
      char colorName[16];
      strncpy(colorName, color.name.c_str(), sizeof(colorName));
      colorName[sizeof(colorName) - 1] = '\0';
      if (((targetId != TARGET_ID_NONE) && (currentTargetId == targetId)) || (targetId == TARGET_ID_NONE)) {
        communication.entityColor(currentTargetId, colorName);
      }
    }
  }
}

GameSessionStatus GameSessionManager::getStatus() {
  return session.status;
}

void GameSessionManager::setStatus(GameSessionStatus newStatus, bool toCommunicate) {
  session.status = newStatus;
  if (toCommunicate) {
    communicateStatus();
  }
}

void GameSessionManager::communicateStatus(uint8_t targetId) {
  if (targetId != TARGET_ID_NONE) {
    communication.sessionStatus(targetId, session.status);
    return;
  }

  for (uint8_t i = 0; i < session.entityCount; ++i) {
    const EntityInfo& entity = session.entities[i];
    for (uint8_t j = 0; j < entity.targetCount; ++j) {
      uint8_t currentTargetId = entity.targetIds[j];
      communication.sessionStatus(currentTargetId, session.status);
    }
  }
}