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

  const bool pairingSollecitation();
  void pairingResponse(const TargetInfo& target);
  void verificationResponse(uint8_t id);
  
  void blinkAll();
  void heartbeatAll();

  const bool scoreUpdate(uint8_t id, ScoreUpdated result);

  // const bool entityColorRequest(uint8_t id, char colorName[16]);
  const bool targetSessionInfoRequest(uint8_t id, const TargetSessionInfo& sessionInfo);
  const bool showTargetColorRequest(uint8_t id, bool switchOn);

  const bool sessionStatusRequest(uint8_t id, GameSessionStatus status);

  const bool litTargetRequest(uint8_t id, uint8_t indexInEntity);
  
private:
  RF24& radio;
  PairingRegistry& registry;

  const bool toTarget(uint8_t id, const void* packet, size_t packetSize, String label = "");
  const bool toAllTargets(const void* packet, size_t packetSize, String label = "");
};

extern Send send;

#endif