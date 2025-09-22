#include "PairingManager.h"
// #include <EEPROM.h>  // ❌ EEPROM removed for stateless pairing
#include "TargetConfig.h"

PairingManager::PairingManager(WirelessManager &wm) : wireless(wm) {}

// ✅ In-memory storage only
uint8_t assignedID = 0xFF;

uint8_t PairingManager::getAssignedID() {
  return assignedID;
}

// ❌ Removed EEPROM-based check
/*
bool PairingManager::isPaired() {
  return EEPROM.read(0) == 1;
}
*/

uint32_t PairingManager::generateToken() {
  uint32_t token = (uint32_t)random(100000, 999999);
  Serial.print(F("🔐 Generated token: "));
  Serial.println(token);
  return token;
}

bool PairingManager::pair() {
  Serial.println(F("🔁 Starting pairing process..."));
  uint32_t token = generateToken();

  const int maxRetries = 3;
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print(F("📨 Attempt "));
    Serial.print(attempt);
    Serial.println(F(" to pair..."));

    wireless.sendPairingRequest(token);

    uint8_t newID;
    if (wireless.receivePairingResponse(newID)) {
      assignedID = newID;  // ✅ Store in memory only

      wireless.switchToTargetPipe(newID);
      Serial.print(F("🔀 Switched to target pipe: TGT"));
      Serial.println(newID);

      Serial.print(F("✅ Paired successfully with ID: "));
      Serial.println(newID);

      return true;
    }

    delay(500);
  }

  Serial.println(F("❌ Pairing failed after multiple attempts."));
  return false;
}

// ❌ Removed verification logic
/*
bool PairingManager::verifyPairing(uint8_t id) {
  Serial.print(F("🔍 Verifying stored ID "));
  Serial.print(id);
  Serial.println(F(" with hub..."));

  const int maxRetries = 2;
  for (int attempt = 1; attempt <= maxRetries; attempt++) {
    Serial.print(F("🔁 Verification attempt "));
    Serial.println(attempt);

    wireless.sendVerificationRequest(id);
    if (wireless.waitForVerificationAck(id)) {
      wireless.switchToTargetPipe(id);
      Serial.println(F("✅ ID verified with hub."));
      return true;
    }

    delay(500);
  }

  Serial.println(F("❌ ID not recognized by hub after retries."));
  return false;
}
*/

// ❌ Removed EEPROM read/write
/*
uint8_t PairingManager::readFlag() {
  return EEPROM.read(0);
}

uint8_t PairingManager::readId() {
  return EEPROM.read(1);
}

void PairingManager::storeAssignedID(uint8_t id) {
  EEPROM.write(0, 1);
  EEPROM.write(1, id);
  Serial.print(F("💾 Stored assigned ID in EEPROM: "));
  Serial.println(id);
}
*/

void PairingManager::clear() {
  assignedID = 0xFF;  // ✅ Just reset in-memory ID
  Serial.println(F("🧹 Cleared in-memory pairing ID."));
}
