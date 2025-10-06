#include "PairingRegistry.h"
#include <EEPROM.h>

#define EEPROM_TOKEN_ADDR 0  // Starting address to store token

PairingRegistry::PairingRegistry(WirelessTarget& wm, Send& send) : wireless(wm), assignedID(0xFF) {}

void PairingRegistry::pair() {
  token = loadTokenFromEEPROM();

  if (token == 0xFFFFFFFF || token == 0) {
    token = random(100000, 999999);
    saveTokenToEEPROM(token);
    Serial.print(F("🆕 Generated new token: "));
    Serial.println(token);
  } else {
    Serial.print(F("🔁 Reusing stored token: "));
    Serial.println(token);
  }

  for (int attempt = 1; attempt <= 3; attempt++) {
    Serial.print(F("📨 Attempt "));
    Serial.print(attempt);
    Serial.println(F(" to pair..."));

    wireless.sendPairingRequest(token);

    // CONTINUE CREATING COMMUNICATION-RECEIVE <--------------------------

    uint8_t id;
    if (wireless.receivePairingResponse(id)) {
      assignedID = id;
      wireless.switchToTargetPipe(id);
      Serial.print(F("✅ Paired successfully with ID: "));
      Serial.println(id);
      return;
    }

    delay(500);
  }

  Serial.println(F("❌ Pairing failed after multiple attempts."));
  assignedID = 0xFF;
}

uint8_t PairingRegistry::getAssignedID() {
  return assignedID;
}

bool PairingRegistry::verify() {
  if (assignedID == 0xFF) {
    Serial.println(F("⚠️ Cannot verify — no assigned ID."));
    return false;
  }

  wireless.sendVerificationRequest(assignedID);
  return wireless.waitForVerificationResponse(assignedID);
}

uint32_t PairingRegistry::loadTokenFromEEPROM() {
  uint32_t storedToken;
  EEPROM.get(EEPROM_TOKEN_ADDR, storedToken);
  return storedToken;
}

void PairingRegistry::saveTokenToEEPROM(uint32_t newToken) {
  EEPROM.put(EEPROM_TOKEN_ADDR, newToken);
}

void PairingRegistry::resetToken() {
  uint32_t blank = 0xFFFFFFFF;
  EEPROM.put(EEPROM_TOKEN_ADDR, blank);
  Serial.println(F("🧹 Token cleared from EEPROM."));
}