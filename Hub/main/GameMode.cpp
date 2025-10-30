#include "GameMode.h"

GameMode::GameMode()
  : name(""), type(ModeType::SINGLE), description(), settingCount(0) {}

GameMode::GameMode(String name, ModeType type, ModeDescription desc, const ModeSetting* settings, uint8_t count)
  : name(name), type(type), description(desc) {
  settingCount = count > MAX_SETTINGS ? MAX_SETTINGS : count;
  for (uint8_t i = 0; i < settingCount; ++i) {
    modeSettings[i] = settings[i];
  }
}

GameMode::GameMode(const GameMode& other)
  : name(other.name),
    type(other.type),
    description(other.description),
    settingCount(other.settingCount) {
  for (uint8_t i = 0; i < settingCount; ++i) {
    modeSettings[i] = other.modeSettings[i];
  }
}

String GameMode::getName() const {
  return name;
}

void GameMode::setName(const String& newName) {
  name = newName;
}

ModeType GameMode::getType() const {
  return type;
}

const ModeDescription& GameMode::getDescription() const {
  return description;
}

const ModeSetting& GameMode::getSetting(uint8_t index) const {
  return modeSettings[index];
}

const ModeSetting (&GameMode::getAllSettings() const)[MAX_SETTINGS] {
  return modeSettings;
}

uint8_t GameMode::getSettingCount() const {
  return settingCount;
}

bool GameMode::isPlayableWith(uint8_t entityCount) const {
  if (type == ModeType::SINGLE) return entityCount >= 1;
  if (type == ModeType::MULTI) return entityCount >= 2;
  return entityCount >= 1;
}

bool GameMode::isValidPlayerTargetCombo(uint8_t players, uint8_t targets) const {
  if (players == 1) return targets >= 1;
  if (players == 2) {
    if (targets == 1) return name == ModeName::Training || name == "Team";
    if (targets >= 2) return true;
  }
  return targets >= players;
}

ModeSetting& GameMode::getMutableSetting(uint8_t index) {
  return modeSettings[index];
}