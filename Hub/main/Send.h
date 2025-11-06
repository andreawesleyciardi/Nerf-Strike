#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <Score.h>
#include <Statuses.h>
#include <Target.h>

#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio, PairingRegistry& registry);

  const bool toTargetPipe(uint8_t id, const uint8_t* pipe, const void* packet, uint8_t length);

  // void pairingResponse(uint8_t assignedID);
  void pairingRequest();
  void pairingResponse(const TargetInfo& target);
  void verificationResponse(uint8_t id);
  
  void blinkAll(PairingRegistry& registry);
  void heartbeatAll(PairingRegistry& registry);

  const bool scoreUpdate(uint8_t id, const uint8_t* pipe, ScoreUpdated result);

  // const bool entityColorRequest(uint8_t id, char colorName[16]);
  const bool targetSessionInfoRequest(uint8_t id, const TargetSessionInfo& sessionInfo);
  const bool showTargetColorRequest(uint8_t id, bool switchOn);

  const bool sessionStatusRequest(uint8_t id, GameSessionStatus status);

  const bool litTargetRequest(uint8_t id, uint8_t indexInEntity);
  
private:
  RF24& radio;
  PairingRegistry& registry;
};

extern Send send;

#endif