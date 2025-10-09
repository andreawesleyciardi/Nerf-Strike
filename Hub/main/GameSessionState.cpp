#include "GameSessionState.h"
#include "PairingRegistry.h"
#include "Color.h"

GameSessionState::GameSessionState() {
  reset();
}

void GameSessionState::reset() {
  entityCount = 0;
  allowedTypeCount = 0;
  optionCount = 0;
  scoreCount = 0;
  selectedGameModeName = "";
  status = SessionStatus::Setting;
}

bool GameSessionState::isMultiplayer() const {
  return entityCount > 1;
}

bool GameSessionState::hasTargetType(TargetType type) const {
  for (uint8_t i = 0; i < allowedTypeCount; ++i) {
    if (allowedTargetTypes[i] == type) return true;
  }
  return false;
}

int GameSessionState::getScoreForTarget(uint8_t targetId) const {
  for (uint8_t i = 0; i < scoreCount; ++i) {
    if (scoreTargetIds[i] == targetId) return scoreValues[i];
  }
  return 0;
}

void GameSessionState::setScore(uint8_t targetId, int value) {
  for (uint8_t i = 0; i < scoreCount; ++i) {
    if (scoreTargetIds[i] == targetId) {
      scoreValues[i] = value;
      return;
    }
  }
  if (scoreCount < MAX_ENTITIES * MAX_TARGETS_PER_ENTITY) {
    scoreTargetIds[scoreCount] = targetId;
    scoreValues[scoreCount] = value;
    scoreCount++;
  }
}

void GameSessionState::assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCountInput, bool useTeams, PairingRegistry& registry) {
  entityCount = 0;
  uint8_t targetsPerEntity = totalTargets / entityCountInput;

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
  return ColorWheel[index % COLOR_WHEEL_SIZE];
}

void GameSessionState::defaultState(PairingRegistry& registry) {
  reset();
  selectedGameModeName = "Training";
  status = SessionStatus::Playing;

  uint8_t pairedIds[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
  uint8_t totalPaired = registry.getPairedTargetIds(pairedIds);

  assignEntitiesBalanced(pairedIds, totalPaired, 1, false, registry);
}