#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <RGBLed.h>
#include "Receive.h"
#include "Send.h"
#include "PairingRegistry.h"
#include "GameLogic.h"

class Communication {
public:
  Communication(Receive& receive, Send& send, PairingRegistry& registry, GameLogic& gameLogic, RGBLed& statusRgbLed);

  void verification(const byte* buffer);
  void pairing(const byte* buffer);
  void hit(const byte* buffer);

private:
  Receive& receive;
  Send& send;
  PairingRegistry& registry;
  GameLogic& gameLogic;
  RGBLed& statusRgbLed;
};

#endif