#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <TargetType.h>
#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio, PairingRegistry& registry);

  void pairingResponse(uint8_t assignedID);
  void verificationResponse(uint8_t id);
  
  void blinkAll(PairingRegistry& registry);
  void heartbeatAll(PairingRegistry& registry);

  void scoreUpdate(uint8_t id, const uint8_t* pipe, uint8_t newScore);

  const bool toTargetPipe(uint8_t id, const uint8_t* pipe, const void* packet, uint8_t length);
private:
  RF24& radio;
  PairingRegistry& registry;
};

extern Send send;

#endif