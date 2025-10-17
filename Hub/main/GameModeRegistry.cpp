#include "GameModeRegistry.h"

GameModeRegistry::GameModeRegistry() {

  // Training
    String singleTrainingDescription[] = { "Normal counter of", "hits." };
    String multiTrainingDescription[]  = { "Normal counter of", "hits." };
    ModeSetting trainingSettings[] = {
      // ModeSetting("Players", 1, 1, 4)
    };
    modes[0] = GameMode(
      ModeName::Training,
      ModeType::ALL,
      ModeDescription(singleTrainingDescription, multiTrainingDescription),
      trainingSettings,
      0
    );

  // ToNumber
    String singleToNumberDescription[] = {"Reach the goal", "number of hits."};
    String multiToNumberDescription[]  = {"Reach first the goal", "number of hits."};
    ModeSetting toNumberSettings[] = {
      ModeSetting("Hits to reach", 5, 1, 50)
    };
    modes[1] = GameMode(
      ModeName::ToNumber,
      ModeType::ALL,
      ModeDescription(singleToNumberDescription, multiToNumberDescription),
      toNumberSettings,
      1
    );

  // Timer
    String singleTimerDescription[] = {"Hit the most targets", "in specific time."};
    String multiTimerDescription[]  = {"Hit the most targets", "in specific time."};
    ModeSetting timerSettings[] = {
      ModeSetting("Time (s)", 60, 10, 300)
    };
    modes[2] = GameMode(
      ModeName::Timer,
      ModeType::ALL,
      ModeDescription(singleTimerDescription, multiTimerDescription),
      timerSettings,
      1
    );

  // TimeForShots
    String singleTimeForShotsDescription[] = {"Hit the most targets", "in specific time."};
    String multiTimeForShotsDescription[]  = {"Hit the most targets", "in specific time."};
    ModeSetting timeForShotsSettings[] = {
      ModeSetting("Shots", 5, 1, 50)
    };
    modes[3] = GameMode(
      ModeName::TimeForShots,
      ModeType::ALL,
      ModeDescription(singleTimeForShotsDescription, multiTimeForShotsDescription),
      timeForShotsSettings,
      1
    );

  // LitTarget
    String singleLitTargetDescription[] = {"Hit the lighted", "target quickly."};
    String multiLitTargetDescription[]  = {"", ""};
    ModeSetting LitTargetsSettings[] = {
      ModeSetting("Time limit (s)", 5, 1, 20)
    };
    modes[4] = GameMode(
      ModeName::LitTarget,
      ModeType::SINGLE,
      ModeDescription(singleLitTargetDescription, multiLitTargetDescription),
      LitTargetsSettings,
      1
    );

  // ModeSetting teamSettings[] = {
    //   ModeSetting("Hits to reach", 10, 1, 100),
    //   ModeSetting("Players", 2, 2, 4)
    // };
    // modes[5] = GameMode(
    //   ModeName::Team,
    //   ModeType::MULTI,
    //   ModeDescription("", "Points are summed to reach goal."),
    //   teamSettings,
    //   2
    // );

  // Battle
    String singleBattleDescription[] = {"", ""};
    String multiBattleDescription[]  = {"Hit target, remove", "opponents points."};
    ModeSetting battleSettings[] = {
      ModeSetting("Hits to reach", 5, 1, 20)
    };
    modes[6] = GameMode(
      ModeName::Battle,
      ModeType::MULTI,
      ModeDescription(singleBattleDescription, multiBattleDescription),
      battleSettings,
      1
    );

  // CrazyTargets
    String singleCrazyTargetsDescription[] = {"", ""};
    String multiCrazyTargetsDescription[]  = {"Targets can randomly", "change color."};
    ModeSetting crazyTargetsSettings[] = {
      ModeSetting("Hits to reach", 5, 1, 50)
    };
    modes[7] = GameMode(
      ModeName::CrazyTargets,
      ModeType::MULTI,
      ModeDescription(singleCrazyTargetsDescription, multiCrazyTargetsDescription),
      crazyTargetsSettings,
      1
    );
}

const GameMode& GameModeRegistry::getMode(uint8_t index) const {
  return modes[index];
}

uint8_t GameModeRegistry::getModeCount() const {
  return MAX_MODES;
}

// uint8_t GameModeRegistry::getFilteredModes(uint8_t entityCount, const GameMode* filteredOut[]) const {
//   uint8_t count = 0;
//   for (uint8_t i = 0; i < MAX_MODES; i++) {
//     if (modes[i].isPlayableWith(pairedTargets)) {
//       filteredOut[count++] = &modes[i];
//     }
//   }
//   return count;
// }

const GameMode& GameModeRegistry::getModeByName(const String& name) const {
  Serial.print("getModeByName name: ");
  Serial.println(name);
  for (uint8_t i = 0; i < MAX_MODES; ++i) {
    Serial.print(F("modes["));
    Serial.print(i);
    Serial.print(F("].getName(): "));
    Serial.println(modes[i].getName());
    if (modes[i].getName() == name) {
      return modes[i];
    }
  }

  return modes[0];
}