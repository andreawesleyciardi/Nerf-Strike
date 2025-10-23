#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include <TargetInfo.h>

#include "Receive.h"
#include "Send.h"
#include "PairingRegistry.h"

class Communication {
public:
  Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed);

  const bool activePairing();
  const bool verification();
  HitResponsePacket hit();
  const String entityColor(const byte* buffer);

private:
  TargetInfo verifyAssignedTarget();  // 🔄 Updated from uint8_t to TargetInfo
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  RGBLed& statusRgbLed;
};

#endif