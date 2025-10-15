#ifndef GAME_SESSION_MANAGER_H
#define GAME_SESSION_MANAGER_H

#include <Arduino.h>
#include "GameSession.h"
#include "PairingRegistry.h"
#include "EntityInfo.h"

class GameSessionManager {
public:
  GameSessionManager();

  // void initializeDefault(PairingRegistry& registry);
  void assignEntitiesBalanced(const uint8_t* pairedTargetIds, uint8_t totalTargets, uint8_t entityCount, bool useTeams, PairingRegistry& registry);
  void restart();

  uint8_t calculateTargetsPerEntity(uint8_t totalPaired = 1, uint8_t totalPlayers = 1, uint8_t totalTeams = 0) const;
  bool isMultiplayer() const;
  int getScoreForEntity(uint8_t entityId) const;
  void setScoreForEntity(uint8_t entityId, int value);
  uint8_t getEntityIdForTarget(uint8_t targetId) const;
  const EntityInfo (&getAllEntities() const)[MAX_ENTITIES];
  uint8_t getEntityCount() const;

  void setSelectedGameMode(const GameMode& mode);
  const GameMode& getSelectedGameMode() const;

  void setSession(const GameSession& newSession);

  GameSession& getSession();
  const GameSession& getSession() const;

private:
  GameSession session;
};

#endif