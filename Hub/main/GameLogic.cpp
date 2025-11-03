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
  // Serial.println(F("Entered in updateEntityScore"));
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
  int updatedScore = calculateScore(gameMode, currentScore);
  sessionManager.setScoreForEntity(entityId, updatedScore);
  ScoreStatus status = evaluateScoreStatus(gameMode, updatedScore);
  // To add a function that updates the session with the status if win or lost

  if (status == ScoreStatus::Won) {
    sessionManager.setStatus(GameSessionStatus::Ended);
  }

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
      if (entities[i].targetCount > 0) {
        if (gameModeName.equals(ModeName::Battle)) {
          // In "Battle" game mode when an entity hits one of its targets subtract a point from the opponents
          int currentOpponentScore = sessionManager.getScoreForEntity(entities[i].entityId);
          int currentOpponentUpdatedScore = currentOpponentScore > 0 ? currentOpponentScore - 1 : 0;
          sessionManager.setScoreForEntity(entities[i].entityId, currentOpponentUpdatedScore);
          for (uint8_t y = 0; y < entities[i].targetCount; ++y) {
            // I add to the batch array the targets that need to be notified that a point is substracted
            batch.updates[batch.count++] = {
              entities[i].targetIds[y],
              static_cast<uint8_t>(currentOpponentUpdatedScore),
              status == ScoreStatus::Won ? ScoreStatus::Lost : ScoreStatus::Subtract
            };
          }
        }
        else {
          if (status == ScoreStatus::Won) {
            int currentOpponentScore = sessionManager.getScoreForEntity(entities[i].entityId);
            for (uint8_t y = 0; y < entities[i].targetCount; ++y) {
              // I add to the batch array the targets that need to be notified that have lost
              batch.updates[batch.count++] = {
                entities[i].targetIds[y],
                static_cast<uint8_t>(currentOpponentScore),
                ScoreStatus::Lost
              };
            }
          }
        }
      }
    }
  }

  return batch;
}

int GameLogic::calculateScore(const GameMode& gameMode, int currentScore) {
  String gameModeName = gameMode.getName();
  if (
    gameModeName.equals(ModeName::Training) ||
    gameModeName.equals(ModeName::ToNumber) ||
    gameModeName.equals(ModeName::Timer) ||
    gameModeName.equals(ModeName::TimeForShots) ||
    gameModeName.equals(ModeName::LitTarget) ||
    gameModeName.equals(ModeName::Battle)
    // || gameModeName.equals(ModeName::CrazyTargets)
  ) {
    return currentScore + 1;
  }
  return 0;
}

ScoreStatus GameLogic::evaluateScoreStatus(const GameMode& gameMode, int score) {
  const ModeSetting* settings = gameMode.getAllSettings();
  String gameModeName = gameMode.getName();

  if (gameModeName.equals(ModeName::Training) || gameModeName.equals(ModeName::Timer)) {
    return ScoreStatus::Add;
  }
  else if (
    gameModeName.equals(ModeName::ToNumber) ||
    gameModeName.equals(ModeName::Battle) ||
    gameModeName.equals(ModeName::LitTarget) ||
    gameModeName.equals(ModeName::TimeForShots)
  ) {
    for (uint8_t i = 0; i < gameMode.getSettingCount(); ++i) {
      if (settings[i].type == SettingType::HITS && score == settings[i].value) {
        return ScoreStatus::Won;
      }
    }
    return ScoreStatus::Add;
  }
  return ScoreStatus::OnGoing;
}

ScoreUpdateBatch GameLogic::gameTimerEnded() {
  sessionManager.setStatus(GameSessionStatus::Ended);

  ScoreUpdateBatch batch;

  const EntityInfo* entities = sessionManager.getAllEntities();
  uint8_t entitiesCount = sessionManager.getEntityCount();

  // Step 1: Find highest score
  int highestScore = 0;
  for (uint8_t i = 0; i < entitiesCount; ++i) {
    int score = sessionManager.getScoreForEntity(entities[i].entityId);
    if (score > highestScore) {
      highestScore = score;
    }
  }

  // Step 2: Collect winners
  uint8_t winners[MAX_ENTITIES];
  uint8_t winnerCount = 0;
  for (uint8_t i = 0; i < entitiesCount; ++i) {
    int score = sessionManager.getScoreForEntity(entities[i].entityId);
    if (score == highestScore) {
      winners[winnerCount++] = entities[i].entityId;
    }
  }

  // Step 3: Notify all targets with correct ScoreStatus
  for (uint8_t i = 0; i < entitiesCount; ++i) {
    int entityScore = sessionManager.getScoreForEntity(entities[i].entityId);
    ScoreStatus status;

    if (winnerCount > 1) {
      status = ScoreStatus::Even;
    } else {
      bool isWinner = false;
      for (uint8_t w = 0; w < winnerCount; ++w) {
        if (entities[i].entityId == winners[w]) {
          isWinner = true;
          break;
        }
      }
      status = isWinner ? ScoreStatus::Won : ScoreStatus::Lost;
    }

    if (entities[i].targetCount > 0) {
      for (uint8_t y = 0; y < entities[i].targetCount; ++y) {
        batch.updates[batch.count++] = {
          entities[i].targetIds[y],
          static_cast<uint8_t>(entityScore),
          status
        };
      }
    }
  }
  
  return batch;
}