#ifndef GAME_SESSION_MANAGER_H
#define GAME_SESSION_MANAGER_H

#include <Arduino.h>
#include "GameSession.h"
#include "PairingRegistry.h"
#include "EntityInfo.h"

class GameSessionManager {
public:
  GameSessionManager(PairingRegistry& registry);

  void assignEntitiesBalanced(bool useTeams = false);
  void restart();

  uint8_t calculateTargetsPerEntity(uint8_t totalPaired, uint8_t totalEntities) const;
  bool isMultiplayer() const;
  int getScoreForEntity(uint8_t entityId) const;
  void setScoreForEntity(uint8_t entityId, int value);
  uint8_t getEntityIdForTarget(uint8_t targetId) const;
  const EntityInfo (&getAllEntities() const)[MAX_ENTITIES];
  
  uint8_t getEntityCount() const;
  void setEntityCount(uint8_t value = 1);

  void setSelectedGameMode(const GameMode& mode);
  const GameMode& getSelectedGameMode() const;

  void setSession(const GameSession& newSession);

  void setStatus(GameSessionStatus newStatus);

  GameSession& getSession();
  const GameSession& getSession() const;

private:
  PairingRegistry& registry;
  GameSession session;
};

#endif