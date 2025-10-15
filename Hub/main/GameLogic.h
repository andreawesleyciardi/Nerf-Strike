#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>
#include <Score.h>
#include "PairingRegistry.h"
#include "GameSessionManager.h"

class GameLogic {
public:
  GameLogic(GameSessionManager& sessionManager);
  void reset();
  ScoreUpdated updateEntityScore(uint8_t targetId);
  int calculateScore(String gameModeName, int currentScore);
  ScoreStatus evaluateScoreStatus(String gameModeName, const GameMode& gameMode, int score = 0);

private:
  GameSessionManager& sessionManager;
};

extern GameLogic gameLogic;

#endif