#ifndef RECEIVE_H
#define RECEIVE_H

#include <Arduino.h>
#include "PairingRegistry.h"

class Receive {
public:
  Receive(RF24& radio, PairingRegistry& registry);

  // const uint8_t pairingResponse(const byte* buffer);
  const uint8_t pairingResponse();
  const bool verificationResponse(uint8_t id);
  const uint8_t* scoreUpdate(const byte* buffer);

private:
  RF24& radio;
  PairingRegistry& registry;
};

#endif