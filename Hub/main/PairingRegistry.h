#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <stdint.h>
#include "HubConfig.h"
// #include <EEPROM.h>  // ❌ Uncomment if using EEPROM

class PairingRegistry {
public:
  PairingRegistry();

  // Pairing logic
  uint8_t assignID(uint32_t token);
  void storePipeForID(uint8_t id, const uint8_t* pipe);
  const uint8_t* getPipeForID(uint8_t id);
  uint8_t getIDAt(uint8_t index);

  // 🔧 Hub command support
  void clearAll();  // 🧹 Clears all pairings

  // ❌ EEPROM-based logic (commented out)
  /*
  void saveToEEPROM();
  void loadFromEEPROM();
  */

private:
  uint8_t idMap[MAX_TARGETS];           // Stores assigned IDs
  uint8_t pipeMap[MAX_TARGETS][5];      // Stores pipe names
};

#endif
