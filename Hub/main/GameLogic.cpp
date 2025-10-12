#include "GameLogic.h"

GameLogic::GameLogic(GameSessionState& session)
  : session(session) {}

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // scores[i] = 0;
  }
}

uint8_t GameLogic::updateEntityScore(uint8_t targetId) {
  if (targetId >= MAX_TARGETS) {       // TO CHECK IF IS CORRECT
    return 0;
  }

  uint8_t entityId = session.getEntityIdForTarget(targetId);
  int currentScore = session.getScoreForEntity(entityId);
  const GameMode& gameMode = session.getSelectedGameMode();
  String gameModeName = gameMode.getName();

  int deltaScore = calculateDeltaScore(gameModeName, currentScore);

  int updatedScore = currentScore;
  session.setScoreForEntity(entityId, updatedScore);
  return updatedScore;
}

int GameLogic::calculateDeltaScore(String gameModeName, int currentScore) {
  if (gameModeName == ModeName::Training) {
    // Training mode logic
  } else if (gameModeName == ModeName::ToNumber) {
    // To number mode logic
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