#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <TargetType.h>
#include <Protocol.h>
#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio);

  void pairingResponse(uint8_t assignedID, TargetType type);
  void verificationResponse(uint8_t id);
  
  void blinkAll(PairingRegistry& registry);
  void heartbeatAll(PairingRegistry& registry);

  void scoreUpdate(uint8_t id, const uint8_t* pipe, uint8_t newScore);

  void toTargetPipe(uint8_t id, const uint8_t* pipe, const void* packet, uint8_t length);
private:
  RF24& radio;
};

extern Send send;

#endif