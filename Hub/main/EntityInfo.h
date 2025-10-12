#ifndef ENTITY_INFO_H
#define ENTITY_INFO_H

#include <Arduino.h>
#include "Color.h"

#define MAX_TARGETS_PER_ENTITY 4

enum class EntityType {
  Player,
  Team
};

struct EntityInfo {
  uint8_t entityId;
  EntityType type;
  Color color;
  int score = 0;

  uint8_t targetIds[MAX_TARGETS_PER_ENTITY];
  uint8_t targetCount = 0;

  String getLabel() const {
    return type == EntityType::Team
      ? "Team " + String(entityId + 1)
      : "Player " + String(entityId + 1);
  }

  void addTarget(uint8_t targetId) {
    if (targetCount < MAX_TARGETS_PER_ENTITY) {
      targetIds[targetCount++] = targetId;
    }
  }

  bool ownsTarget(uint8_t targetId) const {
    for (uint8_t i = 0; i < targetCount; ++i) {
      if (targetIds[i] == targetId) return true;
    }
    return false;
  }

  void reset() {
    score = 0;
    targetCount = 0;
  }
};

#endif