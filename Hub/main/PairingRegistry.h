#ifndef PAIRING_REGISTRY_H
#define PAIRING_REGISTRY_H

#include <stdint.h>
#include <EEPROM.h>

const uint8_t MAX_TARGETS = 10;

class PairingRegistry {
public:
  PairingRegistry();
  uint8_t assignID(uint32_t token);
  bool isTokenPaired(uint32_t token);
  uint8_t getIDForToken(uint32_t token);
  const uint8_t* getPipeForID(uint8_t id);

  uint32_t getTokenAt(uint8_t index);
  uint8_t getIDAt(uint8_t index);
  void clearRegistry();

private:
  struct PairingEntry {
    uint32_t token;
    uint8_t id;
    uint8_t pipe[6];
  };

  PairingEntry registry[MAX_TARGETS];
  uint8_t nextID;

  void loadFromEEPROM();
  void saveEntryToEEPROM(uint8_t index);
};

#endif
