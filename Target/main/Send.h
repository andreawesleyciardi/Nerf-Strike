#ifndef SEND_H
#define SEND_H

#include <Arduino.h>
#include <RF24.h>
#include <TargetType.h>
#include <Protocol.h>
#include "PairingRegistry.h"

class Send {
public:
  Send(RF24& radio);

  void pairingRequest(uint32_t token);
  void verificationRequest(uint8_t assignedID);

  void hitEvent(uint8_t assignedID);

  void toHub(const byte* data, uint8_t length);
private:
  RF24& radio;
};

extern Send send;

#endif