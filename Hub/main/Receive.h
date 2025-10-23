#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include <Protocol.h>
#include <TargetInfo.h>

#include "TargetTypeManager.h"
#include "PairingRegistry.h"

class Receive {
public:
  Receive(TargetTypeManager& typeManager, PairingRegistry& registry);

  const uint8_t* verificationRequest(const byte* buffer);
  // const uint8_t pairingRequest(const byte* buffer);
  TargetInfo pairingRequest(const byte* buffer);
  HitRequestPacket hitRequest(const byte* buffer);

private:
  TargetTypeManager& targetTypeManager;
  PairingRegistry& registry;
};

#endif