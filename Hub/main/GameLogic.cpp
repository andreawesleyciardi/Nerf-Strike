#include "GameLogic.h"
#include "GameMode.h"
#include "EntityInfo.h"
#include "GameModeRegistry.h"
#include "Communication.h"

extern GameModeRegistry gameModeRegistry;
extern Communication communication;

GameLogic::GameLogic(GameSessionManager& sessionManager, PairingRegistry& registry)
  : sessionManager(sessionManager), registry(registry) {}

void GameLogic::reset() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    // scores[i] = 0;
  }
}

// LitTarget
  uint8_t GameLogic::litTargetSelectIndex() {
    const EntityInfo* entities = sessionManager.getAllEntities();
    uint8_t count = sessionManager.getEntityCount();
    for (uint8_t i = 0; i < count; ++i) {
      if (entities[i].targetCount > 1) {
        return random(0, entities[i].targetCount);
      }
    }
    return 0;
  }

  void GameLogic::litTargetSetup() {
    ModeLitTargetState& litState = sessionManager.getLitTargetState();
    const GameMode& gameMode = sessionManager.getSelectedGameMode();
    const ModeSetting* settings = gameMode.getAllSettings();
    uint8_t settingCount = gameMode.getSettingCount();

    DifficultyLevel difficultyLevel = DifficultyLevel::Easy;
    for (uint8_t i = 0; i < settingCount; ++i) {
      if (settings[i].type == SettingType::DIFFICULTY) {
        difficultyLevel = static_cast<DifficultyLevel>(settings[i].value);
        break;
      }
    }

    litState.active = true;
    litState.onDuration = gameModeRegistry.getLitTargetDuration(difficultyLevel);
    litState.startTime = millis();
    litState.index = litTargetSelectIndex();
    // communication.lit(litState.index);                           TODO      in communication.lit create a batch for update all targets                  <------------------------------ PRIORITY
  }

  void GameLogic::litTargetLoop() {
    if (sessionManager.getStatus() != GameSessionStatus::Playing) return;

    ModeLitTargetState& litState = sessionManager.getLitTargetState();
    unsigned long now = millis();

    if (!litState.active) return;

    if (!litState.offPhase && now - litState.startTime >= litState.onDuration) {
      if (sessionManager.allEntitiesHaveSingleTarget()) {
        litState.offPhase = true;
        litState.offDuration = random(500, 1500);
        litState.startTime = now;
      } else {
        litState.index = litTargetSelectIndex();
        // communication.lit(litState.index);                           TODO      in communication.lit create a batch for update all targets                  <------------------------------ PRIORITY
        litState.startTime = now;
      }
    } else if (litState.offPhase && now - litState.startTime >= litState.offDuration) {
      litState.offPhase = false;
      litState.index = litTargetSelectIndex();
      // communication.lit(litState.index);                           TODO      in communication.lit create a batch for update all targets                  <------------------------------ PRIORITY
      litState.startTime = now;
    }
    Serial.print(F("🎯 New selected index: "));
    Serial.println(litState.index);
  }

// Timer
  void GameLogic::timerSetup() {
    const GameMode& gameMode = sessionManager.getSelectedGameMode();
    const ModeSetting* settings = gameMode.getAllSettings();
    uint8_t settingCount = gameMode.getSettingCount();

    for (uint8_t i = 0; i < settingCount; ++i) {
      if (settings[i].type == SettingType::TIME) {
        ModeTimerState& timer = sessionManager.getTimerState();
        timer.duration = settings[i].value * 1000UL;  // convert seconds to ms
        timer.startTime = millis();
        timer.active = true;
        break;
      }
    }
  }

  void GameLogic::timerLoop() {
    if (sessionManager.getStatus() != GameSessionStatus::Playing) return;

    ModeTimerState& timer = sessionManager.getTimerState();
    if (!timer.active) return;

    unsigned long now = millis();
    if (now - timer.startTime >= timer.duration) {
      timer.active = false;
      ScoreUpdateBatch batch = gameTimerEnded();
      if (communication.alertGameTimerEnded(batch)) {
        Serial.println(F("✅ Alerted targets of timer game ended"));
      } else {
        Serial.println(F("❌ Not alerted targets of timer game ended"));
      }
    }
  }

  void GameLogic::timerPause() {
    ModeTimerState& timer = sessionManager.getTimerState();
    if (timer.active) {
      timer.paused = true;
      timer.pauseTime = millis();
    }
  }

  void GameLogic::timerResume() {
    ModeTimerState& timer = sessionManager.getTimerState();
    if (timer.active && timer.paused) {
      unsigned long pausedDuration = millis() - timer.pauseTime;
      timer.startTime += pausedDuration;
      timer.paused = false;
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
  const GameMode& gameMode = sessionManager.getSelectedGameMode();
  String gameModeName = gameMode.getName();

  if (gameModeName.equals(ModeName::LitTarget)) {
    TargetInfo target = registry.getTargetByID(targetId);
    ModeLitTargetState& litState = sessionManager.getLitTargetState();
    // compare target.indexInEntity with the currently selected index in the game session
    // if is not the right index then just return batch empty
    // if is the right index then I think this function can continue execute as it is.
    if (target.indexInEntity != litState.index) {
      return batch;
    }
  }

  int currentScore = sessionManager.getScoreForEntity(entityId);
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
  // String gameModeName = gameMode.getName();
  // if (
  //   gameModeName.equals(ModeName::Training) ||
  //   gameModeName.equals(ModeName::ToNumber) ||
  //   gameModeName.equals(ModeName::Timer) ||
  //   gameModeName.equals(ModeName::TimeForShots) ||
  //   gameModeName.equals(ModeName::LitTarget) ||
  //   gameModeName.equals(ModeName::Battle)
  //   // || gameModeName.equals(ModeName::CrazyTargets)
  // ) {
    return currentScore + 1;
  // }
  // return 0;
}

ScoreStatus GameLogic::evaluateScoreStatus(const GameMode& gameMode, int score) {
  const ModeSetting* settings = gameMode.getAllSettings();
  uint8_t settingCount = gameMode.getSettingCount();

  for (uint8_t i = 0; i < settingCount; ++i) {
    if (settings[i].type == SettingType::HITS && score == settings[i].value) {
      return ScoreStatus::Won;
    }
  }

  return ScoreStatus::Add;
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