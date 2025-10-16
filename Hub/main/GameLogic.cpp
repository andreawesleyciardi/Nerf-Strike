#include "GameLogic.h"
#include "GameMode.h"
#include "EntityInfo.h"

GameLogic::GameLogic(GameSessionManager& sessionManager)
  : sessionManager(sessionManager) {}

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // scores[i] = 0;
  }
}

ScoreUpdateBatch GameLogic::updateEntityScore(uint8_t targetId) {
  Serial.println(F("Entered in updateEntityScore"));
  ScoreUpdateBatch batch;

  if (targetId >= MAX_TARGETS) {
    batch.updates[0] = {targetId, 0, ScoreStatus::Blocked};
    batch.count = 1;
    return batch;
  }

  // Having the id of the hitted target I retreive the assigned entity; after get the current score using the game logic I calculate the new score
  uint8_t entityId = sessionManager.getEntityIdForTarget(targetId);
  int currentScore = sessionManager.getScoreForEntity(entityId);
  const GameMode& gameMode = sessionManager.getSelectedGameMode();
  String gameModeName = gameMode.getName();
  int updatedScore = calculateScore(gameModeName, currentScore);
  sessionManager.setScoreForEntity(entityId, updatedScore);
  ScoreStatus status = evaluateScoreStatus(gameModeName, gameMode, updatedScore);
  // To add a function that updates the session with the status if win or lost

  const EntityInfo* entities = sessionManager.getAllEntities();
  uint8_t entitiesCount = sessionManager.getEntityCount();

  // All the list of entities is scrolled 
  for (uint8_t i = 0; i < entitiesCount; ++i) {
    if (entities[i].entityId == entityId) {
      if (entities[i].targetCount > 0) {
        // All the list of targets assigned to the entity is added to the batch array for get notified of the new score
        for (uint8_t y = 0; y < entities[i].targetCount; ++y) {
          batch.updates[batch.count++] = {
            entities[i].targetIds[y],
            static_cast<uint8_t>(updatedScore),
            status
          };
        }
      }
    }
    else {
      if (gameModeName.equals(ModeName::Battle)) {
        // In "Battle" game mode when an entity hits one of its targets subtract a point from the opponents
        int currentOpponentScore = sessionManager.getScoreForEntity(entities[i].entityId);
        int currentOpponentUpdatedScore = currentOpponentScore > 0 ? currentOpponentScore - 1 : 0;
        sessionManager.setScoreForEntity(entities[i].entityId, currentOpponentUpdatedScore);
        if (entities[i].targetCount > 0) {
          for (uint8_t y = 0; y < entities[i].targetCount; ++y) {
            // I add to the batch array the targets that need to be notified that a point is substracted
            batch.updates[batch.count++] = {
              entities[i].targetIds[y],
              static_cast<uint8_t>(currentOpponentUpdatedScore),
              status == ScoreStatus::Won ? ScoreStatus::Lost : ScoreStatus::Subtract
            };
          }
        }
      }
    }
  }

  return batch;
}

int GameLogic::calculateScore(String gameModeName, int currentScore) {
  Serial.print(F("gameModeName in calculateScore: "));
  Serial.println(gameModeName);
  Serial.print(F("currentScore: "));
  Serial.println(currentScore);
  if (gameModeName.equals(ModeName::Training)) {
    // Training mode logic
    return currentScore + 1;
  } else if (gameModeName.equals(ModeName::ToNumber)) {
    // To number mode logic
    return currentScore + 1;
  } else if (gameModeName.equals(ModeName::Timer)) {
    // Timer mode logic
  } else if (gameModeName.equals(ModeName::TimeForShots)) {
    // Time for shots mode logic
  } else if (gameModeName.equals(ModeName::TwoTargets)) {
    // Two targets mode logic
  } else if (gameModeName.equals(ModeName::Team)) {
    // Team mode logic
  } else if (gameModeName.equals(ModeName::Battle)) {
    // Battle mode logic
    return currentScore + 1;
  } else if (gameModeName.equals(ModeName::CrazyTargets)) {
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
  if (gameModeName.equals(ModeName::Training)) {
    // Training mode logic
  } else if (gameModeName.equals(ModeName::ToNumber)) {
    // To number mode logic
    if (score == settings[0].value) {
      return ScoreStatus::Won;
    } else {
      return ScoreStatus::Add;
    }
  } else if (gameModeName.equals(ModeName::Timer)) {
    // Timer mode logic
  } else if (gameModeName.equals(ModeName::TimeForShots)) {
    // Time for shots mode logic
  } else if (gameModeName.equals(ModeName::TwoTargets)) {
    // Two targets mode logic
  } else if (gameModeName.equals(ModeName::Team)) {
    // Team mode logic
  } else if (gameModeName.equals(ModeName::Battle)) {
    // Battle mode logic
    if (score == settings[0].value) {
      return ScoreStatus::Won;
    } else {
      return ScoreStatus::Add;
    }
  } else if (gameModeName.equals(ModeName::CrazyTargets)) {
    // Crazy targets mode logic
  } else {
    // Unknown or fallback mode logic
  }
  return ScoreStatus::OnGoing;
}