#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include <Target.h>

#include "PairingRegistry.h"

class Receive {
public:
  Receive(RF24& radio, PairingRegistry& registry);

  // const uint8_t pairingResponse(const byte* buffer);
  TargetInfo pairingResponse(uint32_t token);
  const bool verificationResponse(uint8_t id);
  HitResponsePacket hitResponse();
  // const String entityColor(const byte* buffer);
  const bool identifyTargetRequest(const byte* buffer);

private:
  RF24& radio;
  PairingRegistry& registry;
};

#endif