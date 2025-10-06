#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <TargetType.h>
#include <Protocol.h>
#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio, PairingRegistry& registry);
  
  const bool toHub(const byte* data, uint8_t length, bool onTargetPipe = false);

  void pairingRequest(uint32_t token);
  const bool verificationRequest(uint8_t assignedID);

  void hitEvent(uint8_t targetId);

private:
  RF24& radio;
  PairingRegistry& registry;
};

extern Send send;

#endif