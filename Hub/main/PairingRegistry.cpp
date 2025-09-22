#include "PairingRegistry.h"
#include <string.h>
// #include <EEPROM.h>  // ❌ Uncomment if using EEPROM

PairingRegistry::PairingRegistry() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    idMap[i] = 0xFF;
    memset(pipeMap[i], 0, 5);
  }

  // ❌ EEPROM-based loading (commented out)
  // loadFromEEPROM();
}

uint8_t PairingRegistry::assignID(uint32_t token) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (idMap[i] == 0xFF) {
      idMap[i] = i + 1;  // IDs start from 1
      // saveToEEPROM();  // ❌ Uncomment if using EEPROM
      return idMap[i];
    }
  }
  return 0xFF;  // No available ID
}

void PairingRegistry::storePipeForID(uint8_t id, const uint8_t* pipe) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (idMap[i] == id) {
      memcpy(pipeMap[i], pipe, 5);
      // saveToEEPROM();  // ❌ Uncomment if using EEPROM
      return;
    }
  }
}

const uint8_t* PairingRegistry::getPipeForID(uint8_t id) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (idMap[i] == id) {
      return pipeMap[i];
    }
  }
  return nullptr;
}

uint8_t PairingRegistry::getIDAt(uint8_t index) {
  if (index < MAX_TARGETS) {
    return idMap[index];
  }
  return 0xFF;
}

void PairingRegistry::clearAll() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    idMap[i] = 0xFF;
    memset(pipeMap[i], 0, 5);
  }

  // saveToEEPROM();  // ❌ Uncomment if using EEPROM
}

// ❌ EEPROM-based persistence (commented out)
/*
void PairingRegistry::saveToEEPROM() {
  int addr = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    EEPROM.write(addr++, idMap[i]);
    for (uint8_t j = 0; j < 5; j++) {
      EEPROM.write(addr++, pipeMap[i][j]);
    }
  }
}

void PairingRegistry::loadFromEEPROM() {
  int addr = 0;
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    idMap[i] = EEPROM.read(addr++);
    for (uint8_t j = 0; j < 5; j++) {
      pipeMap[i][j] = EEPROM.read(addr++);
    }
  }
}
*/
