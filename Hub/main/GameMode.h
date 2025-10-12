#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <Arduino.h>

enum class ModeType {
  SINGLE,
  MULTI,
  ALL
};

struct ModeName {
  static constexpr const char* Training       = "Training";
  static constexpr const char* ToNumber       = "To number";
  static constexpr const char* Timer          = "Timer";
  static constexpr const char* TimeForShots   = "Time for shots";
  static constexpr const char* TwoTargets     = "Two targets";
  static constexpr const char* Team           = "Team";
  static constexpr const char* Battle         = "Battle";
  static constexpr const char* CrazyTargets   = "Crazy targets";
};

struct ModeDescription {
  String singlePlayerText;
  String multiPlayerText;

  ModeDescription(String singleText, String multiText)
    : singlePlayerText(singleText), multiPlayerText(multiText) {}

  String getTextForPlayers(uint8_t playerCount) const {
    return playerCount > 1 ? multiPlayerText : singlePlayerText;
  }
};

struct ModeSetting {
  String label;
  int value;
  int minValue;
  int maxValue;

  ModeSetting(String label = "", int value = 0, int minValue = 0, int maxValue = 100)
    : label(label), value(value), minValue(minValue), maxValue(maxValue) {}
};

class GameMode {
public:
  GameMode();
  GameMode(String name, ModeType type, ModeDescription desc, const ModeSetting* settings, uint8_t count);

  String getName() const;
  ModeType getType() const;
  const ModeDescription& getDescription() const;
  const ModeSetting& getSetting(uint8_t index) const;
  uint8_t getSettingCount() const;

  bool isPlayableWith(uint8_t pairedTargets) const;
  bool isValidPlayerTargetCombo(uint8_t players, uint8_t targets) const;

private:
  String name;
  ModeType type;
  ModeDescription description;
  ModeSetting modeSettings[5];
  uint8_t settingCount;
};

#endif