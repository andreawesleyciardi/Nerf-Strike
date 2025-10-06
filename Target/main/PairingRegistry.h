#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <Arduino.h>
#include "WirelessTarget.h"

class PairingRegistry {
public:
  PairingRegistry(RF24& radio);

  void setAssignedID(uint8_t newAssignedID);
  uint8_t getAssignedID();
  void setToken(uint32_t newToken);
  void resetToken();

  uint32_t loadTokenFromEEPROM();
  void saveTokenToEEPROM(uint32_t newToken);

  void switchToTargetPipe(uint8_t id);
  const uint8_t* getTargetPipe();

private:
  RF24& radio;
  uint32_t token;
  uint8_t assignedID;
  uint8_t targetPipe[6];
};

#endif
