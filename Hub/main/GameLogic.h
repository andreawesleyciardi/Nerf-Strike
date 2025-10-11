#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>
#include "PairingRegistry.h"
#include "GameSessionState.h"
#include "GameModeRegistry.h"

class GameLogic {
public:
  GameLogic(GameSessionState& session, GameModeRegistry& gameModeRegistry);
  void reset();
  uint8_t updateScoreFor(uint8_t targetId);

private:
  GameSessionState& session;
  GameModeRegistry& gameModeRegistry;
};

extern GameLogic gameLogic;

#endif