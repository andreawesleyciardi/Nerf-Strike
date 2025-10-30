#ifndef GAME_SESSION_MANAGER_H
#define GAME_SESSION_MANAGER_H

#include <Arduino.h>
#include <Statuses.h>
#include <ColorPalette.h>
#include "GameMode.h"
#include "GameSession.h"
#include "PairingRegistry.h"
#include "EntityInfo.h"

constexpr uint8_t TARGET_ID_NONE = 255;

class GameSessionManager {
public:
  GameSessionManager(PairingRegistry& registry);

  void assignEntitiesBalanced(bool useTeams = false);
  void resetSession();
  void restart(bool toCommunicate = true);

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

  GameSessionStatus getStatus();
  void setStatus(GameSessionStatus newStatus, bool toCommunicate = false);
  
  void communicateStatus(uint8_t targetId = TARGET_ID_NONE);
  // void communicateEntityColor(uint8_t targetId = TARGET_ID_NONE);
  void communicateTargetSessionInfo(uint8_t targetId = TARGET_ID_NONE);

  GameMode& getMutableSelectedGameMode();

  GameSession& getSession();
  const GameSession& getSession() const;

private:
  PairingRegistry& registry;
  GameSession session;
};

#endif