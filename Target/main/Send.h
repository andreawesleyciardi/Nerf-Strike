#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <Target.h>
#include <Protocol.h>

#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio, PairingRegistry& registry);
  
  const bool pairingRequest(uint32_t token);
  const bool verificationRequest(uint8_t assignedID);
  const bool heartbeatResponse(uint8_t targetId);
  const bool hitRequest(uint8_t targetId);

private:
  RF24& radio;
  PairingRegistry& registry;

  const bool toHub(const byte* data, uint8_t length, String label = "data", bool logging = true, OnPipe onPipe = OnPipe::targetPipe);
};

extern Send send;

#endif