#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <Arduino.h>
#include "WirelessTarget.h"
#include "Send.h"

class PairingRegistry {
public:
  PairingRegistry(WirelessTarget& wm, Send& send);
  void pair();
  uint8_t getAssignedID();
  void resetToken();
  bool verify();

private:
  WirelessTarget& wireless;
  Send& send;
  uint8_t assignedID;
  uint32_t token;

  uint32_t loadTokenFromEEPROM();
  void saveTokenToEEPROM(uint32_t newToken);
};

#endif
