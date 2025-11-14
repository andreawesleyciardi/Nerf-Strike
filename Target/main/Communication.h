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

  const bool pairingRequest();
  const bool verification();
  // const bool heartbeat();
  HitResponsePacket hit();
  const bool identifyTarget(const byte* buffer);

private:
  TargetInfo verifyAssignedTarget();
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  RGBLed& statusRgbLed;
};

#endif