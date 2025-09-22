#ifndef PAIRING_MANAGER_H
#define PAIRING_MANAGER_H

#include <stdint.h>
#include "WirelessManager.h"

class PairingManager {
public:
  PairingManager(WirelessManager &wm);

  // ✅ Stateless methods
  bool pair();
  uint8_t getAssignedID();

  // ✅ Manual reset (clears in-memory ID)
  void clear();

  // ❌ EEPROM-based methods (commented out)
  /*
  bool isPaired();
  uint8_t readFlag();
  uint8_t readId();
  void storeAssignedID(uint8_t id);
  bool verifyPairing(uint8_t id);
  */

private:
  WirelessManager &wireless;
  uint32_t generateToken();
};

#endif
