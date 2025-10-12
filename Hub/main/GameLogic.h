#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>
#include "PairingRegistry.h"
#include "GameSessionState.h"

class GameLogic {
public:
  GameLogic(GameSessionState& session);
  void reset();
  uint8_t updateEntityScore(uint8_t targetId);
  int calculateDeltaScore(String gameModeName, int currentScore);

private:
  GameSessionState& session;
};

extern GameLogic gameLogic;

#endif