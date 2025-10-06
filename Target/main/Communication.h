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

  void pairing(const byte* buffer);
  void verification(const byte* buffer);
  void hit(const byte* buffer);

private:
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  RGBLed& statusRgbLed;
};

#endif