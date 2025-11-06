#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include <Target.h>

#include "Receive.h"
#include "Send.h"
#include "PairingRegistry.h"

class Communication {
public:
  Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed);

  const bool pairingRequest(uint32_t token = 0xFFFFFFFF);
  const bool verification();
  HitResponsePacket hit();
  // const String entityColor(const byte* buffer);
  const bool showTargetColor(const byte* buffer);

private:
  TargetInfo verifyAssignedTarget();  // 🔄 Updated from uint8_t to TargetInfo
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  RGBLed& statusRgbLed;
};

#endif