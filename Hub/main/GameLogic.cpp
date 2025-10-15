#include "GameLogic.h"
#include "GameMode.h"

GameLogic::GameLogic(GameSessionManager& sessionManager)
  : sessionManager(sessionManager) {}

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // scores[i] = 0;
  }
}

ScoreUpdated GameLogic::updateEntityScore(uint8_t targetId) {
  Serial.println(F("Entered in updateEntityScore"));
  if (targetId >= MAX_TARGETS) {       // TO CHECK IF IS CORRECT
    return {0, ScoreStatus::Lost};
  }

  uint8_t entityId = sessionManager.getEntityIdForTarget(targetId);
  int currentScore = sessionManager.getScoreForEntity(entityId);
  const GameMode& gameMode = sessionManager.getSelectedGameMode();
  String gameModeName = gameMode.getName();
  Serial.println(F("gameModeName in updateEntityScore: "));
  Serial.println(gameModeName);

  int updatedScore = calculateScore(gameModeName, currentScore);
  sessionManager.setScoreForEntity(entityId, updatedScore);

  ScoreStatus status = evaluateScoreStatus(gameModeName, gameMode, updatedScore);
  return {static_cast<uint8_t>(updatedScore), status};
}

int GameLogic::calculateScore(String gameModeName, int currentScore) {
  Serial.print(F("gameModeName in calculateScore: "));
  Serial.println(gameModeName);
  Serial.print(F("currentScore: "));
  Serial.println(currentScore);
  if (gameModeName == ModeName::Training) {
    // Training mode logic
    return currentScore++;
  } else if (gameModeName == ModeName::ToNumber) {
    // To number mode logic
    return currentScore++;
  } else if (gameModeName == ModeName::Timer) {
    // Timer mode logic
  } else if (gameModeName == ModeName::TimeForShots) {
    // Time for shots mode logic
  } else if (gameModeName == ModeName::TwoTargets) {
    // Two targets mode logic
  } else if (gameModeName == ModeName::Team) {
    // Team mode logic
  } else if (gameModeName == ModeName::Battle) {
    // Battle mode logic
  } else if (gameModeName == ModeName::CrazyTargets) {
    // Crazy targets mode logic
  } else {
    // Unknown or fallback mode logic
  }
  return 0;
}

ScoreStatus GameLogic::evaluateScoreStatus(String gameModeName, const GameMode& gameMode, int score) {
  const ModeSetting* settings = gameMode.getAllSettings();
  // for (int i = 0; i < MAX_SETTINGS; ++i) {
  //   Serial.println(settings[i]);
  // }
  Serial.print("gameModeName: ");
  Serial.println(gameModeName);
  if (gameModeName == ModeName::Training) {
    // Training mode logic
  } else if (gameModeName == ModeName::ToNumber) {
    // To number mode logic
    if (score == settings[0].value) {
      ScoreStatus::Win;
    }
  } else if (gameModeName == ModeName::Timer) {
    // Timer mode logic
  } else if (gameModeName == ModeName::TimeForShots) {
    // Time for shots mode logic
  } else if (gameModeName == ModeName::TwoTargets) {
    // Two targets mode logic
  } else if (gameModeName == ModeName::Team) {
    // Team mode logic
  } else if (gameModeName == ModeName::Battle) {
    // Battle mode logic
  } else if (gameModeName == ModeName::CrazyTargets) {
    // Crazy targets mode logic
  } else {
    // Unknown or fallback mode logic
  }
  return ScoreStatus::OnGoing;
}