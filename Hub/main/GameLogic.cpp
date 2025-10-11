#include "GameLogic.h"

GameLogic::GameLogic(GameSessionState& session, GameModeRegistry& gameModeRegistry)
  : session(session), gameModeRegistry(gameModeRegistry) {}

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // scores[i] = 0;
  }
}

uint8_t GameLogic::updateScoreFor(uint8_t targetId) {
  if (targetId >= MAX_TARGETS) {       // TO CHECK IF IS CORRECT
    return 0;
  }

  int currentScore = session.getScoreForTarget(targetId);
  // session.isMultiplayer();
  const GameMode& mode = gameModeRegistry.getModeByName(session.selectedGameModeName);
  currentScore++;
  // DEVELOP GAME LOGIC BASED ON THE MODE

  session.setScore(targetId, currentScore);

  return currentScore;
}