#include "PairingRegistry.h"
#include <Arduino.h>
#include <string.h>

PairingRegistry::PairingRegistry() : nextID(1) {
  loadFromEEPROM();
}

bool PairingRegistry::isTokenPaired(uint32_t token) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry[i].token == token) {
      return true;
    }
  }
  return false;
}

uint8_t PairingRegistry::getIDForToken(uint32_t token) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry[i].token == token) {
      return registry[i].id;
    }
  }
  return 0xFF;
}

const uint8_t* PairingRegistry::getPipeForID(uint8_t id) {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry[i].id == id) {
      return registry[i].pipe;
    }
  }
  return nullptr;
}

uint8_t PairingRegistry::assignID(uint32_t token) {
  if (isTokenPaired(token)) {
    return getIDForToken(token);
  }

  if (nextID >= MAX_TARGETS + 1) {
    Serial.println("⚠️ No available IDs left.");
    return 0xFF;
  }

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    if (registry[i].id == 0xFF) {
      registry[i].token = token;
      registry[i].id = nextID;
      sprintf((char*)registry[i].pipe, "TGT%d", nextID);
      saveEntryToEEPROM(i);

      Serial.print("📋 Assigned ID ");
      Serial.print(nextID);
      Serial.print(" with pipe ");
      Serial.println((char*)registry[i].pipe);

      return nextID++;
    }
  }

  Serial.println("❌ Registry full. Cannot assign ID.");
  return 0xFF;
}

void PairingRegistry::loadFromEEPROM() {
  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    int addr = i * 11;
    EEPROM.get(addr, registry[i].token);
    EEPROM.get(addr + 4, registry[i].id);
    for (uint8_t j = 0; j < 6; j++) {
      registry[i].pipe[j] = EEPROM.read(addr + 5 + j);
    }

    if (registry[i].id != 0xFF && registry[i].id >= nextID) {
      nextID = registry[i].id + 1;
    }
  }
}

void PairingRegistry::saveEntryToEEPROM(uint8_t index) {
  int addr = index * 11;
  EEPROM.put(addr, registry[index].token);
  EEPROM.put(addr + 4, registry[index].id);
  for (uint8_t j = 0; j < 6; j++) {
    EEPROM.write(addr + 5 + j, registry[index].pipe[j]);
  }
}

uint32_t PairingRegistry::getTokenAt(uint8_t index) {
  return registry[index].token;
}

uint8_t PairingRegistry::getIDAt(uint8_t index) {
  return registry[index].id;
}

void PairingRegistry::clearRegistry() {
  Serial.println("🧹 Clearing registry from EEPROM...");
  for (uint8_t i = 0; i < MAX_TARGETS * 11; i++) {
    EEPROM.write(i, 0xFF);
  }

  for (uint8_t i = 0; i < MAX_TARGETS; i++) {
    registry[i].token = 0;
    registry[i].id = 0xFF;
    memset(registry[i].pipe, 0, sizeof(registry[i].pipe));
  }

  nextID = 1;
  Serial.println("✅ Registry cleared. Reboot hub to reload.");
}
