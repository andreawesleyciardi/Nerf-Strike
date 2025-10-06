#include "PairingRegistry.h"
#include <EEPROM.h>

#define EEPROM_TOKEN_ADDR 0  // Starting address to store token

PairingRegistry::PairingRegistry(RF24& radio) : radio(radio), assignedID(0xFF) {}

uint8_t PairingRegistry::getAssignedID() {
  return assignedID;
}

void PairingRegistry::setAssignedID(uint8_t newAssignedID) {
  assignedID = newAssignedID;
  if (newAssignedID != 0xFF) {
    switchToTargetPipe(newAssignedID);
  }
}

void PairingRegistry::setToken(uint32_t newToken) {
  token = newToken;
}

uint32_t PairingRegistry::loadTokenFromEEPROM() {
  uint32_t storedToken;
  EEPROM.get(EEPROM_TOKEN_ADDR, storedToken);
  return storedToken;
}

void PairingRegistry::saveTokenToEEPROM(uint32_t newToken) {
  token = newToken;
  EEPROM.put(EEPROM_TOKEN_ADDR, newToken);
}

void PairingRegistry::resetToken() {
  uint32_t blank = 0xFFFFFFFF;
  EEPROM.put(EEPROM_TOKEN_ADDR, blank);
  Serial.println(F("🧹 Token cleared from EEPROM."));
}

void PairingRegistry::switchToTargetPipe(uint8_t id) {
  sprintf((char*)targetPipe, "TGT%d", id);
  radio.stopListening();
  radio.openReadingPipe(1, targetPipe);
  radio.startListening();

  Serial.print(F("🔀 Switched to target pipe: "));
  Serial.println((char*)targetPipe);
}

const uint8_t* PairingRegistry::getTargetPipe() {
  Serial.print(F("📡 getTargetPipe returns: "));
  for (int i = 0; i < 6; i++) {
    Serial.print((char)targetPipe[i]);
  }
  Serial.println();
  return targetPipe;
}