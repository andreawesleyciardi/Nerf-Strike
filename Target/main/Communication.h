#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include "Receive.h"
#include "Send.h"
#include "PairingRegistry.h"

class Communication {
public:
  Communication(Receive& receive, Send& send, PairingRegistry& registry, RGBLed& statusRgbLed);

  void pairing();
  const bool verification();
  HitResponsePacket hit();
  const String entityColor(const byte* buffer);

private:
  const uint8_t verifyAssignedID();
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  RGBLed& statusRgbLed;
};

#endif