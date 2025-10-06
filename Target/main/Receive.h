#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include <TargetType.h>
#include "PairingRegistry.h"

class Receive {
public:
  Receive(PairingRegistry& registry);

  const uint8_t pairingResponse(const byte* buffer);
  const uint8_t* verificationResponse(const byte* buffer);
  const uint8_t* scoreUpdate(const byte* buffer);

private:
  PairingRegistry& registry;
};

#endif