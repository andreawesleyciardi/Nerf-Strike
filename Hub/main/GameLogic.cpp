#include "GameLogic.h"

GameLogic gameLogic;

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    scores[i] = 0;
  }
}

uint8_t GameLogic::incrementScoreFor(uint8_t targetId) {
  if (targetId < MAX_TARGETS) {
    scores[targetId]++;
    return scores[targetId];
  }
  return 0;
}

uint8_t GameLogic::getScoreFor(uint8_t targetId) {
  if (targetId < MAX_TARGETS) {
    return scores[targetId];
  }
  return 0;
}