#ifndef GAME_MODE_REGISTRY_H
#define GAME_MODE_REGISTRY_H

#include "GameMode.h"

struct ModeTimerState {
  bool active = false;
  bool paused = false;
  unsigned long startTime = 0;
  unsigned long duration = 0;
  unsigned long pauseTime = 0;
};

struct ModeLitTargetState {
  bool active = false;
  bool offPhase = false;
  unsigned long startTime = 0;
  unsigned long onDuration = 0;
  unsigned long offDuration = 0;
  uint8_t index = 0;
};

enum class DifficultyLevel : uint8_t {
  Easy = 1,
  Medium = 2,
  Hard = 3
};

class GameModeRegistry {
public:
  GameModeRegistry();
  const GameMode& getMode(uint8_t index) const;
  uint8_t getModeCount() const;

  // uint8_t getFilteredModes(uint8_t pairedTargets, const GameMode* filteredOut[]) const;

  const GameMode& getModeByName(const String& name) const;

  uint32_t getLitTargetDuration(DifficultyLevel level);

  static const uint8_t MAX_MODES = 6;

private:
  GameMode modes[MAX_MODES];
};

#endif