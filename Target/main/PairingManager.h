#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <Arduino.h>
#include "WirelessManager.h"

class PairingManager {
public:
  PairingManager(WirelessManager& wm);
  void pair();
  uint8_t getAssignedID();
  void resetToken();

private:
  WirelessManager& wireless;
  uint8_t assignedID;
  uint32_t token;

  uint32_t loadTokenFromEEPROM();
  void saveTokenToEEPROM(uint32_t newToken);
};

#endif
