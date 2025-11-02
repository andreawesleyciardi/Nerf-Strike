#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>
#include <Score.h>
#include "PairingRegistry.h"
#include "GameSessionManager.h"

struct ScoreUpdateBatch {
  ScoreUpdatedPerTarget updates[MAX_TARGETS];
  uint8_t count = 0;
};

class GameLogic {
public:
  GameLogic(GameSessionManager& sessionManager);
  void reset();
  ScoreUpdateBatch updateEntityScore(uint8_t targetId);
  int calculateScore(const GameMode& gameMode, int currentScore);
  ScoreStatus evaluateScoreStatus(const GameMode& gameMode, int score = 0);
  ScoreUpdateBatch gameTimerEnded();

private:
  GameSessionManager& sessionManager;
};

extern GameLogic gameLogic;

#endif