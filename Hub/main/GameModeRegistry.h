#ifndef GAME_MODE_REGISTRY_H
#define GAME_MODE_REGISTRY_H

#include "GameMode.h"

class GameModeRegistry {
public:
  GameModeRegistry();
  const GameMode& getMode(uint8_t index) const;
  uint8_t getModeCount() const;

  // uint8_t getFilteredModes(uint8_t pairedTargets, const GameMode* filteredOut[]) const;

  const GameMode& getModeByName(const String& name) const;

  static const uint8_t MAX_MODES = 7;

private:
  GameMode modes[MAX_MODES];
};

#endif