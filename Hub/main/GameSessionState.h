#ifndef GAME_SESSION_STATE_H
#define GAME_SESSION_STATE_H

#include <Arduino.h>
#include "EntityInfo.h"
#include "TargetType.h"

#define MAX_ENTITIES 6
#define MAX_OPTIONS 10

enum class SessionStatus {
  Setting,
  Starting,
  Playing,
  Paused,
  Resetting,
  Won,
  Lost,
  Ended
};

class PairingRegistry;

class GameSessionState {
public:
  GameSessionState();

  void reset();
  void assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCount, bool useTeams, PairingRegistry& registry);

  bool isMultiplayer() const;
  bool hasTargetType(TargetType type) const;

  int getScoreForTarget(uint8_t targetId) const;
  void setScore(uint8_t targetId, int value);

  EntityInfo entities[MAX_ENTITIES];
  uint8_t entityCount = 0;

  TargetType allowedTargetTypes[MAX_ENTITIES];
  uint8_t allowedTypeCount = 0;

  String selectedGameModeName;
  String optionKeys[MAX_OPTIONS];
  int optionValues[MAX_OPTIONS];
  uint8_t optionCount = 0;

  uint8_t scoreTargetIds[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
  int scoreValues[MAX_ENTITIES * MAX_TARGETS_PER_ENTITY];
  uint8_t scoreCount = 0;

  void defaultState(PairingRegistry& registry);

  SessionStatus status;

private:
  Color defaultColorFor(uint8_t index) const;
};

#endif