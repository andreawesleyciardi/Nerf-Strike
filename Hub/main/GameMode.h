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
  static constexpr const char* ToNumber       = "To Number";
  static constexpr const char* Timer          = "Timer";
  static constexpr const char* TimeForShots   = "Time For Shots";
  static constexpr const char* LitTarget      = "Lit Target";
  static constexpr const char* Battle         = "Battle";
  // static constexpr const char* CrazyTargets   = "Crazy Targets";
};

// struct ModeDescription {
//   String singlePlayerText[2];
//   String multiPlayerText[2];

//   ModeDescription() {
//     singlePlayerText[0] = "";
//     singlePlayerText[1] = "";
//     multiPlayerText[0] = "";
//     multiPlayerText[1] = "";
//   }

//   ModeDescription(const String singleText[2], const String multiText[2]) {
//     singlePlayerText[0] = singleText[0];
//     singlePlayerText[1] = singleText[1];
//     multiPlayerText[0] = multiText[0];
//     multiPlayerText[1] = multiText[1];
//   }

//   const String* getDescriptionLines(uint8_t entityCount) const {
//     return entityCount > 1 ? multiPlayerText : singlePlayerText;
//   }
// };

struct ModeDescription {
  String singlePlayerText[2];
  String multiPlayerText[2];

  // Default constructor
  ModeDescription() {
    singlePlayerText[0] = "";
    singlePlayerText[1] = "";
    multiPlayerText[0] = "";
    multiPlayerText[1] = "";
  }

  // Constructor from two arrays of two strings each
  ModeDescription(const String singleText[2], const String multiText[2]) {
    for (uint8_t i = 0; i < 2; ++i) {
      singlePlayerText[i] = singleText[i];
      multiPlayerText[i] = multiText[i];
    }
  }

  // Copy constructor
  ModeDescription(const ModeDescription& other) {
    for (uint8_t i = 0; i < 2; ++i) {
      singlePlayerText[i] = other.singlePlayerText[i];
      multiPlayerText[i] = other.multiPlayerText[i];
    }
  }

  // Assignment operator
  ModeDescription& operator=(const ModeDescription& other) {
    if (this != &other) {
      for (uint8_t i = 0; i < 2; ++i) {
        singlePlayerText[i] = other.singlePlayerText[i];
        multiPlayerText[i] = other.multiPlayerText[i];
      }
    }
    return *this;
  }

  // Accessor based on entity count
  const String* getDescriptionLines(uint8_t entityCount) const {
    return (entityCount > 1) ? multiPlayerText : singlePlayerText;
  }
};


#define MAX_SETTINGS 2

enum class SettingType {
  INT,
  TIME,     // seconds
  HITS,
  // Add more as needed: PERCENT, BOOL, etc.
};

struct ModeSetting {
  String label;
  int value;
  int minValue;
  int maxValue;
  SettingType type;

  ModeSetting(String label = "", int value = 0, int minValue = 0, int maxValue = 100, SettingType type = SettingType::INT)
    : label(label), value(value), minValue(minValue), maxValue(maxValue), type(type) {}
};

class GameMode {
public:
  GameMode();
  GameMode(String name, ModeType type, ModeDescription desc, const ModeSetting* settings, uint8_t count);
  GameMode(const GameMode& other);  // ✅ Copy constructor

  String getName() const;
  void setName(const String& newName);  // ✅ Optional setter

  ModeType getType() const;
  const ModeDescription& getDescription() const;
  const ModeSetting& getSetting(uint8_t index) const;
  const ModeSetting (&getAllSettings() const)[MAX_SETTINGS];
  uint8_t getSettingCount() const;

  bool isPlayableWith(uint8_t entityCount) const;
  bool isValidPlayerTargetCombo(uint8_t players, uint8_t targets) const;

  ModeSetting& getMutableSetting(uint8_t index);

private:
  String name;
  ModeType type;
  ModeDescription description;
  ModeSetting modeSettings[MAX_SETTINGS];
  uint8_t settingCount;
};

#endif