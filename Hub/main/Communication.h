#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include <Statuses.h>
#include <Protocol.h>
#include <OPCodes.h>
#include <DisplayFeedback.h>
#include <Target.h>

#include "Receive.h"
#include "Send.h"
#include "PairingRegistry.h"
#include "GameLogic.h"
#include "GameSessionManager.h"

class Communication {
public:
  Communication(Receive& receive, Send& send, PairingRegistry& registry, GameLogic& gameLogic, RGBLed& statusRgbLed, GameSessionManager& sessionManager);

  void pairing(const byte* buffer);
  void verification(const byte* buffer);
  const bool hit(const byte* buffer);
  const bool targetSessionInfo(uint8_t targetId, const TargetSessionInfo& sessionInfo);
  const bool showTargetColor(uint8_t targetId, bool switchOn = true);
  const bool sessionStatus(uint8_t targetId, GameSessionStatus status);
  const bool alertGameTimerEnded(ScoreUpdateBatch batch);

private:
  const uint8_t* verifyPipeForID(uint8_t targetId);
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  GameLogic& gameLogic;
  RGBLed& statusRgbLed;
  GameSessionManager& sessionManager;
  bool updateBatch(ScoreUpdateBatch batch, uint8_t id = TARGET_ID_NONE);
};

#endif