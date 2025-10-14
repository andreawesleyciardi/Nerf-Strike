#ifndef GAME_SESSION_STATE_H
#define GAME_SESSION_STATE_H

#include <Arduino.h>
#include "EntityInfo.h"
#include "GameMode.h"
#include "Color.h"

#define MAX_ENTITIES 6

const Color EntityColorSequence[] = {
  Color(0,     255,  255,     "Cyan"),        // 0
  Color(255,   0,    0,       "Red"),         // 1
  Color(0,     0,    255,     "Blue"),        // 2
  Color(255,   128,  0,       "Orange"),      // 3
  Color(0,     128,  255,     "Ultramarine"), // 4
  Color(255,   255,  0,       "Yellow"),      // 5
  Color(128,   0,    255,     "Purple"),      // 6
  Color(255,   0,    255,     "Magenta"),     // 7
  Color(0,     255,  0,       "Green"),       // 8
  Color(255,   0,    128,     "Fucsia"),      // 9
  Color(128,   255,  0,       "Lime"),        // 10
  Color(0,     255,  128,     "Seafoam")      // 11
};

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
  uint8_t calculateTargetsPerEntity(uint8_t totalPaired = 1, uint8_t totalPlayers = 1, uint8_t totalTeams = 0);
  void initializeDefault(PairingRegistry& registry);
  // void defaultState(PairingRegistry& registry);

  void assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCount, bool useTeams, PairingRegistry& registry);

  bool isMultiplayer() const;

  int getScoreForEntity(uint8_t entityId) const;
  void setScoreForEntity(uint8_t entityId, int value);
  uint8_t getEntityIdForTarget(uint8_t targetId) const;

  void setSelectedGameMode(const GameMode& mode);
  const GameMode& getSelectedGameMode() const;

  EntityInfo entities[MAX_ENTITIES];
  uint8_t entityCount = 0;

  void restart();

  SessionStatus status;

private:
  Color defaultColorFor(uint8_t index) const;
  GameMode selectedGameMode;
};

#endif