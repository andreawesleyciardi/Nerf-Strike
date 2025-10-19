#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include <Statuses.h>
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
  const bool entityColor(uint8_t targetId, char colorName[16]);
  const bool sessionStatus(uint8_t targetId, GameSessionStatus status);

private:
  const uint8_t* verifyPipeForID(uint8_t targetId);
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  GameLogic& gameLogic;
  RGBLed& statusRgbLed;
  GameSessionManager& sessionManager;
};

#endif