#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include "TargetTypeManager.h"
#include "PairingRegistry.h"

class Receive {
public:
  Receive(TargetTypeManager& typeManager, PairingRegistry& registry);

  const uint8_t* verificationRequest(const byte* buffer);
  const uint8_t pairingRequest(const byte* buffer);
  const uint8_t* hitPacket(const byte* buffer);

private:
  TargetTypeManager& targetTypeManager;
  PairingRegistry& registry;
};

#endif