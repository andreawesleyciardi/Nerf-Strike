#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <Arduino.h>
#include "PairingRegistry.h"

class GameLogic {
public:
  void reset();
  uint8_t incrementScoreFor(uint8_t targetId);
  uint8_t getScoreFor(uint8_t targetId);

private:
  uint8_t scores[MAX_TARGETS] = {0};
};

extern GameLogic gameLogic;

#endif