#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <EEPROM.h>
#include <stdint.h>
#include "WirelessManager.h"

class PairingManager {
public:
  PairingManager(WirelessManager &wm);

  bool isPaired();
  uint8_t getAssignedID();
  void storeAssignedID(uint8_t id);
  uint32_t generateToken();
  bool pair();
  bool verifyPairing(uint8_t id);
  uint8_t readFlag();
  uint8_t readId();
  void clear();

private:
  WirelessManager &wireless;
};

#endif
