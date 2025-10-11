#include "GameModeRegistry.h"

GameModeRegistry::GameModeRegistry() {
  ModeSetting trainingSettings[] = {
    ModeSetting("Players", 1, 1, 4)
  };
  modes[0] = GameMode("Training", ModeType::ALL,
    ModeDescription("Normal counter of hits.", ""),
    trainingSettings, 1);

  ModeSetting toNumberSettings[] = {
    ModeSetting("Hits to reach", 5, 1, 50),
    ModeSetting("Players", 2, 1, 4)
  };
  modes[1] = GameMode("To number", ModeType::ALL,
    ModeDescription("Reach the goal number of hits.", "Who reaches it first wins."),
    toNumberSettings, 2);

  ModeSetting timerSettings[] = {
    ModeSetting("Time (s)", 60, 10, 300)
  };
  modes[2] = GameMode("Timer", ModeType::ALL,
    ModeDescription("Hit the most targets in time.", ""),
    timerSettings, 1);

  ModeSetting timeForShotsSettings[] = {
    ModeSetting("Shots", 5, 1, 50)
  };
  modes[3] = GameMode("Time for shots", ModeType::ALL,
    ModeDescription("Hit selected number of targets fast.", ""),
    timeForShotsSettings, 1);

  ModeSetting twoTargetsSettings[] = {
    ModeSetting("Time limit (s)", 10, 1, 60)
  };
  modes[4] = GameMode("Two targets", ModeType::SINGLE,
    ModeDescription("Hit the lit target quickly.", ""),
    twoTargetsSettings, 1);

  ModeSetting teamSettings[] = {
    ModeSetting("Hits to reach", 10, 1, 100),
    ModeSetting("Players", 2, 2, 4)
  };
  modes[5] = GameMode("Team", ModeType::MULTI,
    ModeDescription("", "Points are summed to reach goal."),
    teamSettings, 2);

  ModeSetting battleSettings[] = {
    ModeSetting("Starting points", 5, 1, 20)
  };
  modes[6] = GameMode("Battle", ModeType::MULTI,
    ModeDescription("", "Hits remove points from opponent."),
    battleSettings, 1);

  ModeSetting crazyTargetsSettings[] = {
    ModeSetting("Hits to reach", 5, 1, 50)
  };
  modes[7] = GameMode("Crazy targets", ModeType::MULTI,
    ModeDescription("", "Targets can randomly invert."),
    crazyTargetsSettings, 1);
}

const GameMode& GameModeRegistry::getMode(uint8_t index) const {
  return modes[index];
}

uint8_t GameModeRegistry::getModeCount() const {
  return MAX_MODES;
}

uint8_t GameModeRegistry::getFilteredModes(uint8_t pairedTargets, const GameMode* filteredOut[]) const {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_MODES; i++) {
    if (modes[i].isPlayableWith(pairedTargets)) {
      filteredOut[count++] = &modes[i];
    }
  }
  return count;
}

const GameMode& GameModeRegistry::getModeByName(const String& name) const {
  for (uint8_t i = 0; i < MAX_MODES; ++i) {
    if (modes[i].getName() == name) {
      return modes[i];
    }
  }

  // Fallback: return first mode or a static default
  return modes[0];  // You could also throw or log an error if needed
}