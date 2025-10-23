#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include <TargetInfo.h>

#include "PairingRegistry.h"

class Receive {
public:
  Receive(RF24& radio, PairingRegistry& registry);

  // const uint8_t pairingResponse(const byte* buffer);
  TargetInfo pairingResponse();
  const bool verificationResponse(uint8_t id);
  HitResponsePacket hitResponse();
  const String entityColor(const byte* buffer);

private:
  RF24& radio;
  PairingRegistry& registry;
};

#endif