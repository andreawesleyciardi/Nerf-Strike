#include "GameMode.h"

GameMode::GameMode() : name(""), type(ModeType::SINGLE), description({"", ""}), settingCount(0) {}

GameMode::GameMode(String name, ModeType type, ModeDescription desc, const ModeSetting* settings, uint8_t count)
  : name(name), type(type), description(desc) {
  settingCount = count > 5 ? 5 : count;
  for (uint8_t i = 0; i < settingCount; i++) {
    modeSettings[i] = settings[i];
  }
}

String GameMode::getName() const { return name; }
ModeType GameMode::getType() const { return type; }
const ModeDescription& GameMode::getDescription() const { return description; }
const ModeSetting& GameMode::getSetting(uint8_t index) const { return modeSettings[index]; }
uint8_t GameMode::getSettingCount() const { return settingCount; }

bool GameMode::isPlayableWith(uint8_t pairedTargets) const {
  if (type == ModeType::SINGLE) return pairedTargets >= 1;
  if (type == ModeType::MULTI) return pairedTargets >= 2;
  return pairedTargets >= 1;
}

bool GameMode::isValidPlayerTargetCombo(uint8_t players, uint8_t targets) const {
  if (players == 1) return targets >= 1;
  if (players == 2) {
    if (targets == 1) return name == "Training" || name == "Team";
    if (targets == 2) return true;
    if (targets >= 3) return true;  // May require exclusion logic
  }
  return targets >= players;
}